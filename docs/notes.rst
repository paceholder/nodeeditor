Random not Categorized Notes
============================

Node Geometry
-------------

.. code-block::

                          spacing  spacing
   spacing               /                \           spacing
           \     port                         port   /
           |  | width  |  |              | | width |  |
           0
         0_|_________________________________________    ___
           /                                         \   ___  spacing
           |                                         |
           |                 Caption                 |        caption height
           |             ________________            |   ___
           |             |               |           |   ___   spacing
           |             |               |           |
           O In Name     |               |  Out Name O        entry
           |             |               |           |   ___
           |             |               |           |   ___  spacing
           |             |               |           |
           O Another In  |               |  Out Name O
           |             |               |           |
           |             |               |           |
           |             |               |           |
           O             |               |           O
           |             |               |           |
           |             |_______________|           |
           |                                         |
           O                                         |
           |                                         |
           \_________________________________________/




Node's size must be recalculated in following cases:

  #. After construction.
  #. Embedding the widget.
  #. After resizing.
  #. Before painting (conditional, depends on whether the font metrics was changed).
  #. When incoming data changed (could trigger size changes, maybe in captions).
  #. When embedded widget changes its size.


