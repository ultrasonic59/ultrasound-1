#include "SystemMessageViewer.h"

#include <QtWidgets>

SystemMessageViewer::SystemMessageViewer(QWidget* parent)
   : QMainWindow(parent), m_view(new QListView), m_model(new QStringListModel)
{
    setWindowFlags(windowFlags() & ~Qt::WindowMinMaxButtonsHint);
    setWindowTitle(QString::fromLocal8Bit("系统消息"));
	resize(350, 600);

	setCentralWidget(m_view);

	QFont new_font = font();
	new_font.setPointSize(font().pointSize() + 1);
	setFont(new_font);

	m_view->setModel(m_model);
	m_view->setWordWrap(true);
	m_view->setEditTriggers(QListView::NoEditTriggers);

	connect(this, SIGNAL(ScrollToBottomSignal()), m_view, SLOT(scrollToBottom()));
}

void SystemMessageViewer::AddMessage(const QString& message) {
	QStringList list = m_model->stringList();
	if (list.count() >= 100) {
		list.removeAt(0);
	}

	if (list.count() > 0) {
		list.append(" ");
	}
	list.append(message);

	m_model->setStringList(list);
	
	emit ScrollToBottomSignal();
}

void SystemMessageViewer::showEvent(QShowEvent * event) {
	QMainWindow::showEvent(event);
	
	m_view->scrollToBottom();
}