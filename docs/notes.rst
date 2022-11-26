Random not Categorized Notes
============================

Node Geometry
-------------

.. code-block::

                         vertical spacing
                        /
            port width                      port width
           |          | |                | |         |

    0 _     _________________________________________    ___
           /                 Caption                 \
           |             ________________            |   ___  caption height
           |             |               |           |
           O In Name     |               |  Out Name O        entry
           |             |               |           |   ___
           |             |               |           |   ___  vertical spacing
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


