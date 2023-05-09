#pragma once
#include <QString>
#include <QJsonObject>
#include <QPushButton>
class ConfigHelper
{
public:
	static enum class JsonObjType
	{
		JSONOBJ_TYPE_UNDEFINE = -1,
		JSONOBJ_TYPE_CAMERA = 0,
		JSONOBJ_TYPE_LOCAL_FILE = 1,
		JSONOBJ_TYPE_MEDIA_SRC
	};

public:
	static ConfigHelper* getInstance();
	static QString getQssString(QString path);
	static void setButtonIcon(QPushButton* pb, int size, QChar icon);
	static void setItemIcon(QWidget* item, int size, QChar icon);


	QJsonObject* getObject(JsonObjType type);
	void initJson();
	int loadJson(const QString filePath);
	int saveJson(const QString filepath);
	int deleteObject(const QString key, JsonObjType type);
	int writeJson(const QString key, QJsonObject object, enum JsonObjType type);

protected:

private:
	ConfigHelper();

private:
	bool is_init_ = false;
	QString file_path_;
	QJsonObject json_src_;
	QJsonObject json_camera_src_;
	QJsonObject json_local_src_;
	QJsonObject json_media_src_;

};

