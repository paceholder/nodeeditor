Data Flow Model
===============

The data flow model automates data propagation between nodes. When a node's
output changes, connected nodes automatically receive the new data. This is
ideal for visual programming, calculators, image processing pipelines, and
similar applications.

.. image:: /_static/screenshots/calculator.png
   :alt: Calculator example showing data flow
   :align: center
   :width: 500px

Overview
--------

The data flow system has three main components:

1. **DataFlowGraphModel** -- Manages nodes and routes data
2. **NodeDelegateModel** -- Your node logic (one class per node type)
3. **NodeDelegateModelRegistry** -- Factory for creating node instances

Setting Up
----------

**1. Define your data type:**

.. code-block:: cpp

   #include <QtNodes/NodeData>

   class NumberData : public QtNodes::NodeData
   {
   public:
       NumberData(double value = 0.0) : _value(value) {}

       // Unique type identifier
       QtNodes::NodeDataType type() const override {
           return {"number", "Number"};
       }

       double value() const { return _value; }

   private:
       double _value;
   };

**2. Create a node delegate:**

.. code-block:: cpp

   #include <QtNodes/NodeDelegateModel>

   class AdditionNode : public QtNodes::NodeDelegateModel
   {
       Q_OBJECT
   public:
       // Identity
       QString caption() const override { return "Add"; }
       QString name() const override { return "Addition"; }

       // Ports
       unsigned int nPorts(PortType type) const override {
           return type == PortType::In ? 2 : 1;
       }

       NodeDataType dataType(PortType, PortIndex) const override {
           return NumberData{}.type();
       }

       // Data handling
       void setInData(std::shared_ptr<NodeData> data, PortIndex port) override;
       std::shared_ptr<NodeData> outData(PortIndex) override;

       // Widget (optional)
       QWidget* embeddedWidget() override { return nullptr; }

   private:
       std::shared_ptr<NumberData> _input1, _input2, _result;
   };

**3. Register nodes and create the model:**

.. code-block:: cpp

   auto registry = std::make_shared<NodeDelegateModelRegistry>();

   // Register with category for menu organization
   registry->registerModel<NumberSourceNode>("Sources");
   registry->registerModel<AdditionNode>("Operators");
   registry->registerModel<DisplayNode>("Outputs");

   DataFlowGraphModel model(registry);
   DataFlowGraphicsScene scene(model);

NodeDelegateModel Methods
-------------------------

**Required methods:**

.. list-table::
   :widths: 30 70
   :header-rows: 1

   * - Method
     - Purpose
   * - ``name()``
     - Unique identifier for serialization
   * - ``caption()``
     - Display name on node
   * - ``nPorts(PortType)``
     - Number of input/output ports
   * - ``dataType(PortType, PortIndex)``
     - Type of data at each port
   * - ``setInData(data, port)``
     - Receive data on input port
   * - ``outData(port)``
     - Provide data from output port
   * - ``embeddedWidget()``
     - Return widget or nullptr

**Optional methods:**

.. list-table::
   :widths: 30 70
   :header-rows: 1

   * - Method
     - Purpose
   * - ``captionVisible()``
     - Hide caption (default: true)
   * - ``portCaption(type, index)``
     - Custom port labels
   * - ``portCaptionVisible(type, index)``
     - Show/hide port labels
   * - ``resizable()``
     - Allow node resizing (default: false)
   * - ``save()`` / ``load()``
     - Custom serialization

Implementing Data Flow
----------------------

**Receiving data (setInData):**

.. code-block:: cpp

   void AdditionNode::setInData(std::shared_ptr<NodeData> data, PortIndex port)
   {
       auto numberData = std::dynamic_pointer_cast<NumberData>(data);

       if (port == 0)
           _input1 = numberData;
       else
           _input2 = numberData;

       // Compute result
       if (_input1 && _input2) {
           double sum = _input1->value() + _input2->value();
           _result = std::make_shared<NumberData>(sum);
       } else {
           _result.reset();  // Invalid if inputs missing
       }

       // Notify downstream nodes
       emit dataUpdated(0);
   }

**Providing data (outData):**

.. code-block:: cpp

   std::shared_ptr<NodeData> AdditionNode::outData(PortIndex)
   {
       return _result;
   }

**Signals to emit:**

- ``dataUpdated(portIndex)`` -- Output data changed, propagate downstream
- ``dataInvalidated(portIndex)`` -- Output is no longer valid

Data Flow Diagram
-----------------

.. image:: /_static/diagrams/dataflow-signals.png
   :alt: Signal flow diagram
   :align: center
   :width: 600px

..
   SCREENSHOT NEEDED: dataflow-signals.png
   Diagram showing the signal/slot flow:

   NodeDelegate A          DataFlowGraphModel         NodeDelegate B
        |                         |                         |
        | dataUpdated(0)          |                         |
        |------------------------>|                         |
        |                         | calls outData(0)        |
        |<------------------------|                         |
        |    returns data         |                         |
        |                         | calls setInData(data,0) |
        |                         |------------------------>|
        |                         |                         |
        |                         |    (B may emit          |
        |                         |     dataUpdated too)    |

   Size: ~600px wide

Embedded Widgets
----------------

Add interactive controls to your nodes:

.. code-block:: cpp

   class NumberSourceNode : public NodeDelegateModel
   {
   public:
       QWidget* embeddedWidget() override
       {
           if (!_spinBox) {
               _spinBox = new QDoubleSpinBox();
               connect(_spinBox, &QDoubleSpinBox::valueChanged,
                       this, &NumberSourceNode::onValueChanged);
           }
           return _spinBox;
       }

   private slots:
       void onValueChanged(double value)
       {
           _data = std::make_shared<NumberData>(value);
           emit dataUpdated(0);  // Push new value downstream
       }

   private:
       QDoubleSpinBox* _spinBox = nullptr;
       std::shared_ptr<NumberData> _data;
   };

.. image:: /_static/screenshots/embedded-widget.png
   :alt: Node with embedded spin box
   :width: 400px

Validation State
----------------

Use ``NodeValidationState`` to indicate whether a node's configuration is valid.
When the state is Warning or Error, a colored icon appears at the node's corner:

- **Valid** (no icon) -- Node is properly configured
- **Warning** (orange icon) -- Some issues, but processing may work
- **Error** (red icon) -- Invalid configuration, cannot process

The ``_stateMessage`` is displayed as a **tooltip** when hovering over the node.

.. code-block:: cpp

   void MyNode::setInData(std::shared_ptr<NodeData> data, PortIndex)
   {
       if (!data) {
           // Show warning with tooltip message
           NodeValidationState state;
           state._state = NodeValidationState::State::Warning;
           state._stateMessage = "Missing input data";  // Shown on hover
           setValidationState(state);
           return;
       }

       if (!isValidInput(data)) {
           // Show error with tooltip message
           NodeValidationState state;
           state._state = NodeValidationState::State::Error;
           state._stateMessage = "Invalid input format";  // Shown on hover
           setValidationState(state);
           return;
       }

       // Input is valid - clear any previous state
       NodeValidationState state;
       state._state = NodeValidationState::State::Valid;
       setValidationState(state);

       // Process data...
   }

Processing Status
-----------------

Use ``NodeProcessingStatus`` to show computation state. A small status icon
appears at the node's corner indicating the current state:

.. image:: /_static/screenshots/processing-status.png
   :alt: Node showing processing status icon
   :align: center
   :width: 500px

.. list-table::
   :widths: 25 75
   :header-rows: 1

   * - Status
     - Meaning
   * - ``NoStatus``
     - Default, no icon shown
   * - ``Processing``
     - Computation in progress (spinner icon)
   * - ``Updated``
     - Successfully completed
   * - ``Pending``
     - Waiting for inputs
   * - ``Empty``
     - No data available
   * - ``Failed``
     - Computation failed
   * - ``Partial``
     - Partially completed (some outputs ready)

.. code-block:: cpp

   void MyNode::compute()
   {
       setNodeProcessingStatus(NodeProcessingStatus::Processing);

       // Start async computation...
       QFuture<Result> future = QtConcurrent::run([this]() {
           return heavyComputation();
       });

       // When done:
       watcher.setFuture(future);
       connect(&watcher, &QFutureWatcher::finished, [this]() {
           setNodeProcessingStatus(NodeProcessingStatus::Updated);
           emit dataUpdated(0);
       });
   }

Type Compatibility
------------------

Connections are only allowed between compatible types. By default, types
must match exactly. Override ``connectionPossible()`` for custom logic:

.. code-block:: cpp

   bool MyModel::connectionPossible(ConnectionId conn) const
   {
       auto outType = portData(conn.outNodeId, PortType::Out,
                               conn.outPortIndex, PortRole::DataType);
       auto inType = portData(conn.inNodeId, PortType::In,
                              conn.inPortIndex, PortRole::DataType);

       // Allow Integer -> Number conversion
       if (outType == "integer" && inType == "number")
           return true;

       return outType == inType;
   }

Complete Example
----------------

See ``examples/calculator/`` for a complete data flow application with:

- Multiple node types (sources, operators, display)
- Embedded widgets
- File save/load
- Custom connection colors

.. seealso::

   - :doc:`graph-models` -- For custom graph implementations
   - :doc:`/examples/index` -- More examples
