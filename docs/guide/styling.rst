Styling
=======

Customize the appearance of nodes, connections, and the canvas.

Style Architecture
------------------

Styles are managed by ``StyleCollection`` and stored as JSON internally:

- **GraphicsViewStyle** -- Canvas background and grid
- **NodeStyle** -- Node colors, borders, fonts
- **ConnectionStyle** -- Connection lines and colors

.. image:: /_static/screenshots/style-example.png
   :alt: Example of styled nodes
   :align: center
   :width: 600px

Setting Styles Globally
-----------------------

Apply styles to all new objects:

.. code-block:: cpp

   #include <QtNodes/StyleCollection>
   #include <QtNodes/NodeStyle>
   #include <QtNodes/ConnectionStyle>
   #include <QtNodes/GraphicsViewStyle>

   // From JSON string
   NodeStyle::setNodeStyle(R"({
       "NodeStyle": {
           "NormalBoundaryColor": [255, 255, 255],
           "SelectedBoundaryColor": [255, 200, 0],
           "GradientColor0": [40, 40, 40],
           "GradientColor1": [60, 60, 60],
           "GradientColor2": [50, 50, 50],
           "GradientColor3": [45, 45, 45],
           "ShadowColor": [20, 20, 20],
           "ShadowEnabled": true,
           "FontColor": "white",
           "PenWidth": 2.0,
           "Opacity": 0.9
       }
   })");

GraphicsViewStyle
-----------------

Controls the canvas appearance:

.. code-block:: json

   {
     "GraphicsViewStyle": {
       "BackgroundColor": [53, 53, 53],
       "FineGridColor": [60, 60, 60],
       "CoarseGridColor": [25, 25, 25]
     }
   }

.. list-table::
   :widths: 30 70
   :header-rows: 1

   * - Property
     - Description
   * - ``BackgroundColor``
     - Main canvas background
   * - ``FineGridColor``
     - Small grid lines
   * - ``CoarseGridColor``
     - Major grid lines

NodeStyle
---------

Controls node appearance:

.. code-block:: json

   {
     "NodeStyle": {
       "NormalBoundaryColor": [255, 255, 255],
       "SelectedBoundaryColor": [255, 165, 0],
       "GradientColor0": "gray",
       "GradientColor1": [80, 80, 80],
       "GradientColor2": [64, 64, 64],
       "GradientColor3": [58, 58, 58],
       "ShadowColor": [20, 20, 20],
       "ShadowEnabled": true,
       "FontColor": "white",
       "FontColorFaded": "gray",
       "ConnectionPointColor": [169, 169, 169],
       "FilledConnectionPointColor": "cyan",
       "ErrorColor": [211, 47, 47],
       "WarningColor": [255, 179, 0],
       "ToolTipIconColor": "white",
       "PenWidth": 1.0,
       "HoveredPenWidth": 1.5,
       "ConnectionPointDiameter": 8.0,
       "Opacity": 0.8
     }
   }

.. list-table::
   :widths: 30 70
   :header-rows: 1

   * - Property
     - Description
   * - ``NormalBoundaryColor``
     - Border when not selected
   * - ``SelectedBoundaryColor``
     - Border when selected
   * - ``GradientColor0-3``
     - Background gradient stops
   * - ``ShadowEnabled``
     - Drop shadow on/off
   * - ``ConnectionPointColor``
     - Empty port circles
   * - ``FilledConnectionPointColor``
     - Connected port circles
   * - ``ErrorColor``
     - Validation error indicator
   * - ``WarningColor``
     - Validation warning indicator

ConnectionStyle
---------------

Controls connection line appearance:

.. code-block:: json

   {
     "ConnectionStyle": {
       "ConstructionColor": "gray",
       "NormalColor": "darkcyan",
       "SelectedColor": [100, 100, 100],
       "SelectedHaloColor": "orange",
       "HoveredColor": "lightcyan",
       "LineWidth": 3.0,
       "ConstructionLineWidth": 2.0,
       "PointDiameter": 10.0,
       "UseDataDefinedColors": false
     }
   }

.. list-table::
   :widths: 30 70
   :header-rows: 1

   * - Property
     - Description
   * - ``ConstructionColor``
     - Color while dragging new connection
   * - ``NormalColor``
     - Default connection color
   * - ``SelectedColor``
     - Selected connection
   * - ``HoveredColor``
     - Mouse hover color
   * - ``UseDataDefinedColors``
     - Color by data type (see below)

Data-Defined Connection Colors
------------------------------

Color connections based on data type:

.. code-block:: cpp

   ConnectionStyle::setConnectionStyle(R"({
       "ConnectionStyle": {
           "UseDataDefinedColors": true
       }
   })");

Then define colors in your ``NodeDataType``:

.. code-block:: cpp

   NodeDataType NumberData::type() const
   {
       return NodeDataType{
           "number",           // id
           "Number",           // name
           QColor(0, 128, 255) // color for connections
       };
   }

.. image:: /_static/screenshots/connection-colors.png
   :alt: Colored connections by data type
   :width: 400px

..
   SCREENSHOT NEEDED: connection-colors.png
   - Show connection_colors example
   - Multiple data types with different colored connections
   - Clear visual distinction between types
   - Size: ~400px wide

Per-Node Styling
----------------

Override styles for individual nodes via ``NodeRole::Style``:

.. code-block:: cpp

   QVariant MyModel::nodeData(NodeId nodeId, NodeRole role) const
   {
       if (role == NodeRole::Style) {
           NodeStyle style = StyleCollection::nodeStyle();

           // Custom color for this node
           if (isErrorNode(nodeId)) {
               style.GradientColor0 = QColor(100, 40, 40);
           }

           return style.toJson().toVariantMap();
       }
       // ...
   }

Or in a ``NodeDelegateModel``:

.. code-block:: cpp

   class MyNode : public NodeDelegateModel
   {
   public:
       MyNode()
       {
           // Set background color
           setBackgroundColor(QColor(60, 100, 60));
       }
   };

Loading Styles from File
------------------------

Load styles from external JSON:

.. code-block:: cpp

   QFile file("my_style.json");
   if (file.open(QIODevice::ReadOnly)) {
       QByteArray data = file.readAll();
       QString json = QString::fromUtf8(data);

       NodeStyle::setNodeStyle(json);
       ConnectionStyle::setConnectionStyle(json);
       GraphicsViewStyle::setGraphicsViewStyle(json);
   }

Example Style File
------------------

Complete style file (``my_style.json``):

.. code-block:: json

   {
     "GraphicsViewStyle": {
       "BackgroundColor": [30, 30, 30],
       "FineGridColor": [40, 40, 40],
       "CoarseGridColor": [20, 20, 20]
     },
     "NodeStyle": {
       "NormalBoundaryColor": [100, 100, 100],
       "SelectedBoundaryColor": [255, 180, 0],
       "GradientColor0": [50, 50, 55],
       "GradientColor1": [45, 45, 50],
       "GradientColor2": [40, 40, 45],
       "GradientColor3": [35, 35, 40],
       "ShadowEnabled": true,
       "FontColor": [220, 220, 220],
       "PenWidth": 1.5,
       "Opacity": 0.95
     },
     "ConnectionStyle": {
       "NormalColor": [80, 180, 220],
       "SelectedColor": [255, 180, 0],
       "LineWidth": 2.5,
       "UseDataDefinedColors": true
     }
   }

.. seealso::

   - ``examples/styles/`` -- Style customization example
   - ``examples/connection_colors/`` -- Data-defined colors example
