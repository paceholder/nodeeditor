Library Usage
=============

Overview
--------

QtNodes is a Qt5-based library designed for graphical representation of
the node graphs and performing various operations on them.

.. image:: /_static/calculator.png

The project is built with the CMake configurator. Therefore it is
quite easy to incorporate the library into any CMake-based Qt project.

As of version `3.0` the library uses the Model-View approach. The
central class `GraphModel` is designed for inheriting and extending by
the user. It should wrap the data representing the graph and forward it
to the `BasicGraphicsScene` -- a class responsible for populating
`QGraphicsObject` items and showing them in the `QGraphicsView`
widgets.
