#pragma once
#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QListView;
class QStringListModel;
QT_END_NAMESPACE

class SystemMessageViewer : public QMainWindow
{
    Q_OBJECT

public:
	SystemMessageViewer(QWidget* parent = nullptr);
	void AddMessage(const QString& message);

signals:
	void ScrollToBottomSignal();

protected:
	void showEvent(QShowEvent * event) override;

private:
	QListView * m_view;
	QStringListModel* m_model;
};
