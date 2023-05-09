#ifndef VOLUME_SLIDER_DIALOG_H
#define VOLUME_SLIDER_DIALOG_H

#include <QDialog>

class CustomSlider;

class VolumeSliderDialog : public QDialog
{
	Q_OBJECT

public:
	VolumeSliderDialog(QWidget *parent = nullptr);
	~VolumeSliderDialog();

	void setSliderValue(int value);
	void setMute(bool status);

protected:
	bool event(QEvent* event) override;


public slots:
	void onVolumeMute(bool status);
signals:
	void sigSliderValueChanged(int value);

private:
	CustomSlider* slider_ = nullptr;
	int volume_ = 100;
	bool flip_ = false;
};

#endif