#pragma once

#include <QWidget>
#include <QPushButton>

#include <QtNodes/Definitions>

#include <QVBoxLayout>
#include <QHBoxLayout>


using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeId;

class DynamicPortsModel;


/**
 *                PortAddRemoveWidget
 *
 * ```
 *       _left                         _right
 *       layout                        layout
 *     ----------------------------------------
 *     |         |                  |         |
 *     | [+] [-] |                  | [+] [-] |
 *     |         |                  |         |
 *     | [+] [-] |                  | [+] [-] |
 *     |         |                  |         |
 *     | [+] [-] |                  | [+] [-] |
 *     |         |                  |         |
 *     | [+] [-] |                  |         |
 *     |         |                  |         |
 *     |_________|__________________|_________|
 * ```
 *
 * The widget has two main vertical layouts containing groups of buttons for
 * adding and removing ports. Each such a `[+] [-]` group is contained in a
 * dedicated QHVBoxLayout.
 *
 */
class PortAddRemoveWidget : public QWidget
{
  Q_OBJECT
public:
  PortAddRemoveWidget(unsigned int nInPorts,
                      unsigned int nOutPorts,
                      NodeId nodeId,
                      DynamicPortsModel & model,
                      QWidget * parent = nullptr);

  ~PortAddRemoveWidget();


  /**
   * Called from constructor, creates all button groups according to models'port
   * counts.
   */
  QHBoxLayout*
  populateButtons(QVBoxLayout* verticalLayout, unsigned int nPorts);

  /**
   * Adds a single `[+][-]` button group to a given layout.
   */
  QHBoxLayout*
  addButtonGroupToLayout(QVBoxLayout * vbl, unsigned int portIndex);

  /**
   * Removes a single `[+][-]` button group from a given layout.
   */
  QHBoxLayout*
  removeButtonGroupFromLayout(QVBoxLayout * vbl, unsigned int portIndex);

private Q_SLOTS:
  void onPlusClicked();

  void onMinusClicked();

private:
  /**
   * @param buttonIndex is the index of a button in the layout.
   * Plus button has the index 0.
   * Minus button has the index 1.
   */
  std::pair<PortType, PortIndex>
  findWhichPortWasClicked(QObject* sender, int const buttonIndex);

private:
  NodeId const _nodeId;
  DynamicPortsModel & _model;

  QVBoxLayout* _left;
  QVBoxLayout* _right;
};
