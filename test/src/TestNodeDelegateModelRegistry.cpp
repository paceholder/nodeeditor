#include <QtNodes/NodeDelegateModel>
#include <QtNodes/NodeDelegateModelRegistry>

#include <catch2/catch.hpp>

using QtNodes::NodeDelegateModel;
using QtNodes::NodeDelegateModelRegistry;

namespace {
class TestModelWithStaticName : public NodeDelegateModel
{
public:
    static QString Name() { return "StaticNameModel"; }
    QString name() const override { return "StaticNameModel"; }
    QString caption() const override { return "Static Name Model"; }
    unsigned int nPorts(QtNodes::PortType) const override { return 0; }
    QtNodes::NodeDataType dataType(QtNodes::PortType, QtNodes::PortIndex) const override { return {}; }
    void setInData(std::shared_ptr<QtNodes::NodeData>, QtNodes::PortIndex const) override {}
    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex const) override { return nullptr; }
    QWidget* embeddedWidget() override { return nullptr; }
};

class TestModelWithName : public NodeDelegateModel
{
public:
    TestModelWithName(const QString &name = "DefaultName")
        : _modelName(name)
    {}
    
    QString name() const override { return _modelName; }
    QString caption() const override { return QString("Model: %1").arg(_modelName); }
    unsigned int nPorts(QtNodes::PortType) const override { return 0; }
    QtNodes::NodeDataType dataType(QtNodes::PortType, QtNodes::PortIndex) const override { return {}; }
    void setInData(std::shared_ptr<QtNodes::NodeData>, QtNodes::PortIndex const) override {}
    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex const) override { return nullptr; }
    QWidget* embeddedWidget() override { return nullptr; }

private:
    QString _modelName;
};
} // namespace

TEST_CASE("NodeDelegateModelRegistry registration and creation", "[registry]")
{
    NodeDelegateModelRegistry registry;

    SECTION("Register model with static Name() method")
    {
        registry.registerModel<TestModelWithStaticName>();
        
        auto model = registry.create("StaticNameModel");
        REQUIRE(model != nullptr);
        CHECK(model->name() == "StaticNameModel");
        CHECK(model->caption() == "Static Name Model");
    }

    SECTION("Register model with category")
    {
        registry.registerModel<TestModelWithName>("CustomCategory");
        
        auto model = registry.create("DefaultName");
        REQUIRE(model != nullptr);
        CHECK(model->name() == "DefaultName");
        CHECK(model->caption() == "Model: DefaultName");
    }

    SECTION("Register with lambda factory")
    {
        registry.registerModel(
            []() { return std::make_unique<TestModelWithName>("LambdaModel"); },
            "LambdaCategory"
        );
        
        auto model = registry.create("LambdaModel");
        REQUIRE(model != nullptr);
        CHECK(model->name() == "LambdaModel");
        CHECK(model->caption() == "Model: LambdaModel");
    }

    SECTION("Create non-existent model")
    {
        auto model = registry.create("NonExistentModel");
        CHECK(model == nullptr);
    }
}

TEST_CASE("NodeDelegateModelRegistry categories", "[registry]")
{
    NodeDelegateModelRegistry registry;

    SECTION("Register models with categories")
    {
        registry.registerModel<TestModelWithStaticName>("Category1");
        registry.registerModel<TestModelWithName>("Category2");
        
        auto categories = registry.categories();
        bool foundCategory1 = false;
        bool foundCategory2 = false;
        
        for (const auto &cat : categories) {
            if (cat == "Category1") {
                foundCategory1 = true;
            }
            if (cat == "Category2") {
                foundCategory2 = true;
            }
        }
        
        CHECK(foundCategory1);
        CHECK(foundCategory2);
        CHECK(categories.size() >= 2);
    }

    SECTION("Registered model names")
    {
        registry.registerModel<TestModelWithStaticName>();
        registry.registerModel<TestModelWithName>("CustomCategory");
        
        auto creators = registry.registeredModelCreators();
        bool foundStatic = false;
        bool foundDefault = false;
        
        for (const auto &pair : creators) {
            if (pair.first == "StaticNameModel") {
                foundStatic = true;
            }
            if (pair.first == "DefaultName") {
                foundDefault = true;
            }
        }
        
        CHECK(foundStatic);
        CHECK(foundDefault);
        CHECK(creators.size() >= 2);
    }
}

TEST_CASE("NodeDelegateModelRegistry models by category", "[registry]")
{
    NodeDelegateModelRegistry registry;

    registry.registerModel<TestModelWithStaticName>("Inputs");
    registry.registerModel<TestModelWithName>("Outputs");
    
    SECTION("Get models by existing category")
    {
        auto inputModels = registry.registeredModelsCategoryAssociation();
        
        // Check that our categories exist  
        bool foundInputs = false;
        bool foundOutputs = false;
        
        for (const auto &pair : inputModels) {
            if (pair.first == "StaticNameModel" && pair.second == "Inputs") {
                foundInputs = true;
            }
            if (pair.first == "DefaultName" && pair.second == "Outputs") {
                foundOutputs = true;
            }
        }
        
        CHECK(foundInputs);
        CHECK(foundOutputs);
    }
}
