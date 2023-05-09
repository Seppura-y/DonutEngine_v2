#pragma once
#include <QMenuBar>
#include <QPushButton>
class MenuBar : public QMenuBar
{
	Q_OBJECT

public :
	MenuBar(QWidget* parent = nullptr);
	~MenuBar();

	class FileMenu : public QMenu
	{
	public:
		FileMenu();
		~FileMenu();
		QAction* act_open_local_, act_open_web_cmr_, act_open_cmr_;
	};

	class OptionMenu : public QMenu
	{
	public:
		OptionMenu();
		~OptionMenu();
		QAction* act_setting_, act_render_;
	};

	class AboutMenu : public QMenu
	{
	public:
		AboutMenu();
		~AboutMenu();
		QAction* act_about_qt_;
	};

signals:
	void sigExtendButtonClicked();
public slots:
	void onExtendButtonClicked();
	
public:
	FileMenu* menu_file_;
	OptionMenu* menu_opt_;
	AboutMenu* menu_about_;
	QPushButton* pb_extend_;
};

