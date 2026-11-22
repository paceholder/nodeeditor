Planned changes for v4: node groups
===================================

Node groups were added in 3.1.4. They work, but their design has limits.
Version 4 will rebuild groups so that the model owns the group data, like
it does for nodes and connections. This page lists the known problems of
groups in 3.x and says why they are not fixed there.

.. note::

   The v4 design is not final. Progress is tracked in
   `issue #539 <https://github.com/paceholder/nodeeditor/issues/539>`_.

What is wrong with groups in 3.x
--------------------------------

The scene owns the groups. The model does not know about them. A group is a
``NodeGroup`` that holds raw pointers to node graphics items, and the scene
changes node positions and group state directly. This is against the rule of
the library: the model is the source of truth, and the scene follows it.

The model has no group data, so:

- A group is not saved by ``DataFlowGraphModel::save()``. It is saved by the
  scene.
- Creating a group, adding a node to it and removing a node from it cannot be
  undone.
- Undo of a node deletion brings the node back outside its group. A group
  that was deleted and restored loses its name and its id.
- Copy and paste of a group loses its name and lock state.

The graphics items point to each other, so:

- A node that is dragged over a group keeps a raw pointer to it. If the group
  is deleted during the drag, the pointer dangles.
- The drag-into-group state is reset only while the node still touches the
  group. If you move a node away from a group in one big step and release it,
  the node joins the group.
- The group item changes its own geometry inside ``paint()``. That schedules
  the next paint, so a view with a group repaints all the time, also when
  idle. This costs CPU.

Other problems:

- ``BasicGraphicsScene::loadItems()`` with a paste position moves plain nodes
  in the graphics only, and the model keeps the old position. The offset of a
  group is wrong, because the group item is always at (0,0). The built-in
  paste does not use this path, so only code that calls ``loadItems()`` itself
  is affected.
- Two separate drags of the same node are merged into one undo step. This is
  the behavior of the upstream code. A fix needs a decision on what one undo
  step is.

Why these are not fixed in 3.x
------------------------------

Each of these problems comes from the same cause: the group state is in the
scene. A correct fix moves it to the model. That changes the public API
(``NodeGroup``, ``childNodes()`` and the group functions of the scene), so it
needs a new major version.

A local fix in 3.x would rewrite code that v4 removes. It would also add more
code to a design we want to leave. So 3.x gets only the fixes that matter to
users today:

- Node moves go through the model again (#516).
- A locked node, or a node of a locked group, can no longer start connections
  (#536).
- Crashes in the group lifetime are fixed (#540): a model reset with a group,
  the double delete of the group item, and a node in more than one group.
- ``GroupGraphicsObject`` is exported. On a Windows shared build, code that
  called its methods did not link.

Use of groups in 3.x
--------------------

Groups are usable. Until v4:

- Do not rely on undo for group changes.
- Do not call ``BasicGraphicsScene::loadItems()`` with a paste position.
- Keep the group lock on, if you do not want the drag-into-group behavior.

What changes in v4
------------------

- The model holds the group data: a ``GroupId`` per node, and name and lock
  per group. A small helper class, which a model keeps as a member, does the
  bookkeeping, so each model does not need its own.
- ``NodeGroup`` and the pointers between graphics items are removed.
  ``GroupGraphicsObject`` becomes a view that draws a rectangle around the
  nodes of the group.
- Group changes are undo commands.
- Groups do not nest and cannot be folded.
- Saved scenes keep the ``"groups"`` array, so files of 3.x load in v4.
