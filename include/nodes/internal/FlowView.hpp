#pragma once

#include <QtWidgets/QGraphicsView>

#include "Export.hpp"

class QMenu;
class QMimeData;

namespace QtNodes
{

class FlowScene;
class NodeGraphicsObject;
class GroupGraphicsObject;

/**
 * @brief The FlowView class controls the display of the FlowScene in the UI.
 * It is responsible for mediating the user interaction with the scene and
 * managing the UI overlays, such as context menus for nodes.
 */
class NODE_EDITOR_PUBLIC FlowView
  : public QGraphicsView
{
  Q_OBJECT
public:

  FlowView(QWidget *parent = Q_NULLPTR);
  FlowView(FlowScene *scene, QWidget *parent = Q_NULLPTR);

  FlowView(const FlowView&) = delete;
  FlowView operator=(const FlowView&) = delete;

  QAction* clearSelectionAction() const;

  QAction* deleteSelectionAction() const;

  /**
   * @brief Returns the "copy selection" action.
   */
  QAction* copySelectionAction() const;

  /**
   * @brief Returns the "cut selection" action.
   */
  QAction* cutSelectionAction() const;

  /**
   * @brief Returns the "paste from clipboard" action.
   */
  QAction* pasteClipboardAction() const;

  /**
   * @brief Returns the "create group from selection" action.
   */
  QAction* createGroupFromSelectionAction() const;

  /**
   * @brief Returns the "load group from file" action.
   */
  QAction* loadGroupAction() const;

  /**
   * @brief Converts the given MIME data to a JSON document. The result
   * should be checked with isEmpty() before usage. This function is auxiliary to
   * the getClipboardAsJson() function and the drag/drop functionality.
   * @param data Mime data to be converted to JSON
   * @return A byte array formatted as JSON if the contents of the given MIME data
   * were valid, an empty byte array otherwise.
   */
  QByteArray mimeToJson(const QMimeData* mimeData) const;

  void setScene(FlowScene *scene);

  /**
   * @brief _clipboardMimeType Stores the MIME type that will be used by the View
   * to copy and paste scene objects. Currently set to "application/json".
   */
  static const QString _clipboardMimeType;

public Q_SLOTS:

  void scaleUp();

  void scaleDown();

  void deleteSelectedNodes();

  /**
   * @brief Manages the available actions when there is a change in the
   * scene's selected items.
   */
  void handleSelectionChanged();

protected:
  /**
   * @brief Creates a context menu when the user right-clicks a group area.
   * @param event Event generated by Qt when the context menu is opened.
   * @param ggo Graphical object of the group that was clicked.
   */
  void groupContextMenu(QContextMenuEvent *event, GroupGraphicsObject* ggo);

  /**
   * @brief Creates a context menu when the user right-clicks a node.
   * @param event Event generated by Qt when the context menu is opened.
   * @param ngo Graphical object of the node that was clicked.
   */
  void nodeContextMenu(QContextMenuEvent *event, NodeGraphicsObject* ngo);

  /**
   * @brief Copies the selected scene items to the clipboard.
   */
  void copySelectionToClipboard();

  /**
   * @brief Cuts the selected scene items to the clipboard.
   */
  void cutSelectionToClipboard();

  /**
   * @brief Pastes the clipboard items on the scene.
   */
  void pasteFromClipboard();

  /**
   * @brief Handles a "load group from file" action, setting where the group should
   * be positioned and calling the appropriate flow scene function.
   */
  void handleLoadGroup();

  /**
   * @brief Properly restores any pasted or dropped files on the scene.
   * @param filepath Path to a .group or .flow file
   * @param pos Desired group's position, in scene coordinates. Doesn't
   * affect .flow files.
   */
  void handleFilePaste(const QString& filepath, const QPointF& pos);

  /**
   * @brief Loads to the scene any .group and .flow files in the specified
   * mime data.
   * @param mimeData Mime data that might contain copied files. In case
   * there are no files, this function does nothing.
   * @param pos Desired group's position, in scene coordinates. Doesn't
   * affect .flow files.
   */
  void loadFilesFromMime(const QMimeData* mimeData, const QPointF& pos);

  /**
   * @brief Checks if the given mime data contains valid file URLs for the scene.
   * If at least one valid file is passed, the result is true. This checks neither
   * the existence nor the contents of the files, and is used when enabling paste
   * and drop actions.
   * @param mimeData Data to be checked
   * @return true if at least one of the files is valid, false otherwise.
   */
  bool checkMimeFiles(const QMimeData* mimeData) const;

  void gentleZoom(double factor);

  void contextMenuEvent(QContextMenuEvent *event) override;

  void wheelEvent(QWheelEvent *event) override;

  void keyPressEvent(QKeyEvent *event) override;

  void keyReleaseEvent(QKeyEvent *event) override;

  void mousePressEvent(QMouseEvent *event) override;

  void mouseMoveEvent(QMouseEvent *event) override;

  void drawBackground(QPainter* painter, const QRectF& r) override;

  void showEvent(QShowEvent *event) override;

  void dragEnterEvent(QDragEnterEvent* event) override;

  void dragMoveEvent(QDragMoveEvent* event) override;

  void dropEvent(QDropEvent* event) override;

protected:

  FlowScene * scene();

private:

  QAction* _clearSelectionAction;
  QAction* _deleteSelectionAction;

  /**
   * @brief _copySelectionAction Action that triggers copying the selected scene items to the clipboard.
   */
  QAction* _copySelectionAction;

  /**
   * @brief _cutSelectionAction Action that triggers cutting the selected scene items to the clipboard.
   */
  QAction* _cutSelectionAction;

  /**
   * @brief _pasteClipboardAction Action that triggers pasting of the stored clipboard on the scene.
   */
  QAction* _pasteClipboardAction;

  /**
   * @brief _createGroupFromSelectionAction Action that triggers the creation of a new group from the
   * selected items.
   */
  QAction* _createGroupFromSelectionAction;

  /**
   * @brief _loadGroupAction Action to load a node group from a .group file.
   */
  QAction* _loadGroupAction;

  /**
   * @brief _clickPos Stores the last mouse press position.
   */
  QPointF _clickPos;

  /**
   * @brief _lastPastePos Stores the last viewport center of a paste action, if the action wasn't
   * triggered on a specific scene position. Used to avoid overlapping items when multiple paste
   * actions are taken without changing the viewport.
   */
  QPointF _lastPastePos{};

  /**
   * @brief _pasteCount Counts the number of paste actions taken in a row without moving the scene
   * viewport. Used to avoid overlapping items.
   */
  int _pasteCount{};

  FlowScene* _scene;

  /**
   * @brief _clipboard A pointer to the application's clipboard.
   */
  QClipboard* _clipboard;

  double _currentZoomFactor{1.0};

  static constexpr double _zoomFactor{1.25};

  static constexpr std::pair<double, double> _zoomLimits{0.01, 2.0};

  /**
   * @brief _pastePosOffset Determines the position offset when the paste action is taken several
   * times in a row.
   */
  static constexpr int _pastePosOffset{20};
};
}
