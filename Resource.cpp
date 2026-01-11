#include <iostream>
#include <vector>
#include <functional>

class Resource
{
public:
    virtual ~Resource() = default;
    
    virtual std::string getName() const = 0;

    Resource(const Resource&) = delete;
    Resource& operator=(const Resource&) = delete;
    
    Resource(Resource&&) = default;
    Resource& operator=(Resource&&) = default;

protected:
    Resource() = default;
};

class TestSource : public Resource
{
public:
    std::string name_;
    std::vector<int> data_;
    size_t hash_;
    std::hash<std::string> HashFanc;

    inline static int totalConstruct_ = 0;
    inline static int totalDestruct_ = 0;

    TestSource(std::string name, int size) : 
        name_(name),
        hash_(HashFanc(name)),
        data_(size, 0) 
        {
            ++totalConstruct_;
            std::cout << "Constructed: " << name << " (id: " << hash_ << ")\n";
        }

    TestSource(TestSource&& other) noexcept :
        name_(std::move(other.name_)),
        data_(std::move(other.data_)),
        hash_(other.hash_)
        {
            other.hash_ = 0;
        }

    TestSource& operator=(TestSource&& other) noexcept
    {
        if (this != &other) {
            name_ = std::move(other.name_);
            data_ = std::move(other.data_);
            hash_ = other.hash_;
            other.hash_ = 0;
        }

        return *this;
    }

    ~TestSource() {
        ++totalDestruct_;
        std::cout << "Destructed: " << name_ << "\n";
    }

    std::string getName() const override { return this->name_; }

    size_t getHash() const { return hash_; }
    size_t getDataSize() const { return data_.size(); }

    static int getTotalConstructions() { return totalConstruct_; }
    static int getTotalDestructions() { return totalDestruct_; }
    static void resetCounters() {
        totalConstruct_ = 0;
        totalDestruct_ = 0;
    };
};

void testBasic() {
    TestSource::resetCounters();
    
    std::cout << "\n=== Test 1: Construction ===" << std::endl;
    {
        TestSource r1("texture1", 1000);
        std::cout << "Constructions: " << TestSource::getTotalConstructions() << std::endl;
        std::cout << "Destructions: " << TestSource::getTotalDestructions() << std::endl;
    }
    std::cout << "After destruction - Destructions: " << TestSource::getTotalDestructions() << std::endl;
    
    std::cout << "\n=== Test 2: Move Semantics ===" << std::endl;
    TestSource::resetCounters();
    
    TestSource r1("texture2", 1000);
    int constrBefore = TestSource::getTotalConstructions();
    
    TestSource r2 = std::move(r1);  // Перемещение
    
    std::cout << "r2 name: " << r2.getName() << std::endl;
    std::cout << "r1 name: " << r1.getName() << std::endl;  // Пусто после move!
    std::cout << "Constructions after move: " << TestSource::getTotalConstructions() << std::endl;
    std::cout << "(Should be same as before: " << constrBefore << ")" << std::endl;
}

int main() {
    testBasic();
}