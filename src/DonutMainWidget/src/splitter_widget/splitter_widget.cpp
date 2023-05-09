#include "splitter_widget.h"

#include <QScreen>
#include <QGridLayout>
#include <QLabel>
#include <QDebug>
#include <QScrollBar>

#include "splitter_scroll_area.h"
#include "render_widget.h"

static QPoint evpoint;
static double scale;

SplitterWidget::SplitterWidget(QWidget* parent) : QWidget(parent)
{
	//this->setWindowFlag(Qt::FramelessWindowHint);

	initUi();
	show();
	startTimer(10);
	is_controler_mode_ = false;
	//this->setStyleSheet("background-color: orange;");
}

SplitterWidget::~SplitterWidget()
{
}

void SplitterWidget::initUi()
{
	initSplitterContent();
	//resetSplitterContent();
	onResetSplitterContent(1);
}

void SplitterWidget::initSplitterContent()
{
	scroll_area_ = new SplitterScrollArea();
	scroll_area_->setMergeEnable(false);
	scroll_area_->setSplitEnable(false);

	QVBoxLayout* main_layout = new QVBoxLayout();
	this->setLayout(main_layout);
	main_layout->setSpacing(1);
	main_layout->setContentsMargins(0, 0, 0, 0);
	main_layout->addWidget(scroll_area_);

	grid_layout_ = new QGridLayout(content_widget_);
	grid_layout_->setSpacing(1);
	grid_layout_->setContentsMargins(0, 0, 0, 0);

	content_widget_ = new QWidget();

	QGridLayout* scroll_layout = new QGridLayout();
	scroll_area_->setLayout(scroll_layout);
	scroll_area_->setWidget(content_widget_);

	content_widget_->setLayout(grid_layout_);
	scroll_area_->setWidgetResizable(false);
	content_widget_->show();

	first_point_selected_ = false;
	second_point_selected_ = false;

	QObject::connect(scroll_area_, SIGNAL(sigMerge()), this, SLOT(onMerge()));
	QObject::connect(scroll_area_, SIGNAL(sigSplit()), this, SLOT(onSplit()));
	QObject::connect(scroll_area_, SIGNAL(sigSave()), this, SLOT(onSave()));
	QObject::connect(scroll_area_, SIGNAL(sigClear()), this, SLOT(onClear()));
	QObject::connect(scroll_area_, SIGNAL(sigLoad()), this, SLOT(onLoad()));
	//QObject::connect(scroll_area_, SIGNAL(sigRstsvr()), this, SLOT(onResetSvr()));
	//QObject::connect(scroll_area_, SIGNAL(sigSetaddr()), this, SLOT(onSetAddress()));
	//QObject::connect(scroll_area_, SIGNAL(sigSendfile()), this, SLOT(onSendLayoutFile()));
}

void SplitterWidget::resetSplitterContent()
{
	if (is_need_init_ &&
		(controler_width_ != -1) &&
		(controler_height_ != -1) &&
		((controler_width_ != controler_width_prev_) || (controler_height_ != controler_height_prev_)))
	{
		for (int i = 0; i < grid_rows_; i++)
		{
			for (int j = 0; j < grid_colums_; j++)
			{
				QLayoutItem* item = grid_layout_->itemAtPosition(i, j);
				if (!item)continue;
				int index = grid_layout_->indexOf(item);
				RenderWidget* item_widget = (RenderWidget*)grid_layout_->takeAt(index)->widget();
				if (!item_widget)continue;
				delete item_widget;
			}
		}
		render_widgets_.clear();

		int scrollbar_height = scroll_area_->getHorizontalScrollbarHeight();
		double scale = (double)(scroll_area_->height() - scrollbar_height) / controler_height_;

		//scroll_area_->setWidget(content_widget_);
		//content_widget_->show();

		if (is_controler_mode_)
		{
			grid_rows_ = controler_height_ / 100;
			grid_colums_ = controler_width_ / (controler_height_ / grid_rows_);
		}
		else
		{
			//if (is_customlayout_mode_)
			{
				controler_width_ = this->width();
				controler_height_ = this->height() - scrollbar_height;

				grid_rows_ = 12;
				grid_colums_ = controler_width_ / ((controler_height_ - scrollbar_height) / grid_rows_);
			}
			//else
			{

			}
		}

		for (int i = 0; i < grid_rows_; i++)
		{
			for (int j = 0; j < grid_colums_; j++)
			{
				RenderWidget* wid = new RenderWidget(i, j);
				QObject::connect(wid, SIGNAL(sigSelected(int, int)), this, SLOT(onRefreshUi(int, int)));
				grid_layout_->addWidget(wid, i, j);
				render_widgets_.insert({ std::make_pair(i, j), wid});
			}
		}

		//controler_width_prev_ = controler_width_;
		//controler_height_prev_ = controler_height_;
		is_need_init_ = false;
	}
}


void SplitterWidget::onSelectedUpdate()
{
}

void SplitterWidget::resizeEvent(QResizeEvent* ev)
{
	int scrollbar_height = scroll_area_->getHorizontalScrollbarHeight();
	int hvalue = scroll_area_->horizontalScrollBar()->value();
	int vvalue = scroll_area_->verticalScrollBar()->value();
	QPoint topLeft = scroll_area_->viewport()->rect().topLeft();

	double scale = 1.0;
	if (is_controler_mode_)
	{
		scale = (double)(scroll_area_->height() - scrollbar_height) / controler_height_;
		scroll_area_->widget()->resize(controler_width_ * scale, controler_height_);
	}
	else
	{
		scroll_area_->widget()->resize(this->width() * scale, this->height());
	}

	scroll_area_->widget()->move(topLeft.x() - hvalue, topLeft.y() - vvalue);

	if (!is_content_init_)
	{
		is_content_init_ = true;
		//resetSplitterContent();
	}

	return QWidget::resizeEvent(ev);
}

void SplitterWidget::timerEvent(QTimerEvent* ev)
{
	if (is_need_init_)
	{
		//resetSplitterContent();
	}
}

void SplitterWidget::mousePressEvent(QMouseEvent* ev)
{
	if (ev->button() == Qt::LeftButton)
	{
		evpoint = ev->pos();
		is_leftbutton_pressed_ = true;
	}
}

void SplitterWidget::mouseMoveEvent(QMouseEvent* ev)
{
	//if (!is_leftbutton_pressed_)
	{
		return QWidget::mouseMoveEvent(ev);
	}
	//if (evpoint.y() > ui.wHead->height())
	//{
	//	return QWidget::mouseMoveEvent(ev);
	//}
	//this->move(ev->globalPos() - evpoint);
}

void SplitterWidget::mouseReleaseEvent(QMouseEvent* ev)
{
	is_leftbutton_pressed_ = false;
	return QWidget::mouseReleaseEvent(ev);
}

void SplitterWidget::clearSelection()
{
	first_point_selected_ = false;
	second_point_selected_ = false;
	first_colum_ = -1;
	first_row_ = -1;
	second_colum_ = -1;
	second_row_ = -1;

	start_colum_ = -1;
	start_row_ = -1;
	selected_width_ = -1;
	selected_height_ = -1;
	end_colum_ = -1;
	end_row_ = -1;
}

void SplitterWidget::onMerge()
{
	first_point_selected_ = false;
	first_colum_ = -1;
	first_row_ = -1;
	second_point_selected_ = false;
	second_colum_ = -1;
	second_row_ = -1;

	for (int i = 0; i < grid_rows_; i++)
	{
		for (int j = 0; j < grid_colums_; j++)
		{
			RenderWidget* render_widget = (RenderWidget*)grid_layout_->itemAtPosition(i, j)->widget();
			if (render_widget == nullptr)continue;
			if (render_widget->isSelected())
			{
				render_widget->setFirstPoint(false);
				render_widget->setSelected(false);
				render_widget->onRefreshUi();
			}

		}
	}

	QLayoutItem* item = grid_layout_->itemAtPosition(start_row_, start_colum_);
	int index = grid_layout_->indexOf(item);
	RenderWidget* render_widget = (RenderWidget*)grid_layout_->takeAt(index)->widget();
	render_widgets_.erase(std::make_pair(start_row_,start_colum_));
	if (render_widget)
	{
		delete render_widget;
		render_widget = nullptr;
	}

	for (int i = start_row_; i < start_row_ + selected_height_; i++)
	{
		for (int j = start_colum_; j < start_colum_ + selected_width_; j++)
		{
			if (i == start_row_ && j == start_colum_)continue;
			item = grid_layout_->itemAtPosition(i, j);
			if (!item)continue;
			render_widget = (RenderWidget*)item->widget();
			if (!render_widget)continue;

			if ((render_widget->getWidth() > 1) || (render_widget->getHeight() > 1))
			{
				int index = grid_layout_->indexOf(item);
				grid_layout_->takeAt(index);
				render_widgets_.erase(std::make_pair(i, j));
				delete render_widget;
				render_widget = new RenderWidget(i, j);
				grid_layout_->addWidget(render_widget, i, j);
				render_widgets_.insert({ std::make_pair(i, j),render_widget });
			}

			render_widget->setCovered(true);
			render_widgets_.erase(std::make_pair(i, j));
			render_widget->setBlockRow(start_row_);
			render_widget->setBlockColum(start_colum_);
		}
	}

	render_widget = new RenderWidget(start_row_, start_colum_, selected_width_, selected_height_);
	QObject::connect(render_widget, SIGNAL(sigSelected(int, int)), this, SLOT(onRefreshUi(int, int)));
	grid_layout_->addWidget(render_widget, start_row_, start_colum_, selected_height_, selected_width_);
	render_widgets_.insert({ std::make_pair(start_row_, start_colum_),render_widget });

	scroll_area_->setMergeEnable(false);
	scroll_area_->setSplitEnable(false);
}

void SplitterWidget::onSplit()
{
	qDebug() << "spliting first row : " << first_row_ << "  first colum : " << first_colum_;
	//int index = -1;
	int width = -1, height = -1;
	int row = first_row_;
	int colum = first_colum_;
	QLayoutItem* item = nullptr;
	RenderWidget* item_widget = nullptr;


	item = grid_layout_->itemAtPosition(row, colum);
	int index = grid_layout_->indexOf(item);
	item_widget = (RenderWidget*)grid_layout_->takeAt(index)->widget();
	if (item_widget == nullptr)return;
	width = item_widget->getWidth();
	height = item_widget->getHeight();
	delete item_widget;
	render_widgets_.erase(std::make_pair(row, colum));

	item_widget = new RenderWidget(row, colum);
	QObject::connect(item_widget, SIGNAL(sigSelected(int, int)), this, SLOT(onRefreshUi(int, int)));
	grid_layout_->addWidget(item_widget, row, colum);
	render_widgets_.insert({ std::make_pair(row, colum), item_widget });
	scroll_area_->setMergeEnable(false);
	scroll_area_->setSplitEnable(false);

	for (int i = row; i < row + height; i++)
	{
		for (int j = colum; j < colum + width; j++)
		{
			item = grid_layout_->itemAtPosition(i, j);
			//if (!item)continue;
			item_widget = (RenderWidget*)item->widget();
			//if (!item_widget)continue;

			if (item_widget->isCovered())
			{
				item_widget->setCovered(false);
				render_widgets_.insert({ std::make_pair(i, j), item_widget });
				item_widget->setBlockRow(-1);
				item_widget->setBlockColum(-1);
			}
		}
	}

	//item = grid_layout_->itemAtPosition(row, colum);
	//int index = grid_layout_->indexOf(item);
	//item_widget = (ElWidget*)grid_layout_->takeAt(index)->widget();
	//if (item_widget == nullptr)return;
	//delete item_widget;

	//item_widget = new ElWidget(row, colum);
	//QObject::connect(item_widget, SIGNAL(sigSelected(int, int)), this, SLOT(refreshing(int, int)));
	//grid_layout_->addWidget(item_widget, row, colum);
	//scroll_area_->set_merge_enable(false);
	//scroll_area_->set_split_enable(false);

	first_point_selected_ = false;
	first_row_ = -1;
	first_colum_ = -1;
	second_point_selected_ = false;
	second_row_ = -1;
	second_colum_ = -1;
}

void SplitterWidget::onClear()
{
	first_point_selected_ = false;
	first_row_ = -1;
	first_colum_ = -1;

	second_point_selected_ = false;
	second_row_ = -1;
	second_colum_ = -1;

	int index = -1;
	QLayoutItem* item;
	RenderWidget* render_widget;
	for (int i = 0; i < grid_rows_; i++)
	{
		for (int j = 0; j < grid_colums_; j++)
		{
			item = grid_layout_->itemAtPosition(i, j);
			if (item == nullptr)
			{
				render_widget = new RenderWidget(i, j);
				QObject::connect(render_widget, SIGNAL(sigSelected(int, int)), this, SLOT(onRefreshUi(int, int)));
				grid_layout_->addWidget(render_widget, i, j);
				render_widgets_.insert({ std::make_pair(i, j), render_widget });
			}
			index = grid_layout_->indexOf(item);
			render_widget = (RenderWidget*)grid_layout_->takeAt(index)->widget();
			delete render_widget;

			render_widget = new RenderWidget(i, j);
			QObject::connect(render_widget, SIGNAL(sigSelected(int, int)), this, SLOT(onRefreshUi(int, int)));
			grid_layout_->addWidget(render_widget, i, j);
			render_widgets_.insert({ std::make_pair(i, j), render_widget });
		}
	}
}

void SplitterWidget::onSave()
{
}

void SplitterWidget::onLoad()
{
}


void SplitterWidget::onRefreshUi(int row, int colum)
{
	QLayoutItem* item = nullptr;
	RenderWidget* item_widget = nullptr;
	if (!first_point_selected_)
	{
		scroll_area_->setMergeEnable(false);
		first_point_selected_ = true;
		first_colum_ = colum;
		first_row_ = row;
		item_widget = (RenderWidget*)grid_layout_->itemAtPosition(first_row_, first_colum_)->widget();
		if (item_widget == nullptr)return;
		if ((item_widget->getWidth() > 1) || (item_widget->getHeight() > 1))
		{
			scroll_area_->setSplitEnable();
		}
		if (item_widget->isSelected())
		{
			item_widget->setFirstPoint(true);
			item_widget->onRefreshUi();
		}
		return;
	}
	if (!second_point_selected_)
	{
		second_point_selected_ = true;
		scroll_area_->setMergeEnable(true);
		scroll_area_->setSplitEnable(false);
	}
	else
	{
		for (int i = 0; i < grid_rows_; i++)
		{
			for (int j = 0; j < grid_colums_; j++)
			{
				item_widget = (RenderWidget*)grid_layout_->itemAtPosition(i, j)->widget();
				if (item_widget == nullptr)continue;
				if (item_widget->isSelected())
				{
					if (item_widget->isFirstPoint())continue;
					item_widget->setSelected(false);
					item_widget->onRefreshUi();
				}

			}
		}
	}
	second_colum_ = colum;
	second_row_ = row;

	// Unselect
	if (second_colum_ == first_colum_ && second_row_ == first_row_)
	{
		first_point_selected_ = false;
		second_point_selected_ = false;
		item_widget = (RenderWidget*)grid_layout_->itemAtPosition(first_row_, first_colum_)->widget();
		item_widget->setFirstPoint(false);
		item_widget->setSelected(false);
		item_widget->onRefreshUi();
		first_colum_ = -1;
		first_row_ = -1;
		second_colum_ = -1;
		second_row_ = -1;
		scroll_area_->setMergeEnable(false);
		scroll_area_->setSplitEnable(false);
		return;
	}
	item_widget = (RenderWidget*)grid_layout_->itemAtPosition(first_row_, first_colum_)->widget();
	int end_colum_1 = item_widget->getColum() + item_widget->getWidth();
	int end_row_1 = item_widget->getRow() + item_widget->getHeight();

	item_widget = (RenderWidget*)grid_layout_->itemAtPosition(second_row_, second_colum_)->widget();
	int end_colum_2 = item_widget->getColum() + item_widget->getWidth();
	int end_row_2 = item_widget->getRow() + item_widget->getHeight();

	end_colum_ = (end_colum_1 - end_colum_2) > 0 ? end_colum_1 : end_colum_2;
	end_row_ = (end_row_1 - end_row_2) > 0 ? end_row_1 : end_row_2;


	start_colum_ = (first_colum_ - second_colum_) < 0 ? first_colum_ : second_colum_;
	start_row_ = (first_row_ - second_row_) < 0 ? first_row_ : second_row_;

	selected_width_ = end_colum_ - start_colum_;
	selected_height_ = end_row_ - start_row_;


	for (;;)
	{
		int temp_colum = start_colum_;
		int temp_row = start_row_;
		int temp_end_colum = end_colum_;
		int temp_end_row = end_row_;
		int temp_width = selected_width_;
		int temp_height = selected_height_;
		getRange();
		if (temp_colum == start_colum_ && temp_row == start_row_ && temp_end_colum == end_colum_ && temp_end_row == end_row_ && temp_width == selected_width_ && temp_height == selected_height_)
		{
			break;
		}
	}

	for (int i = start_row_; i < start_row_ + selected_height_; i++)
	{
		for (int j = start_colum_; j < start_colum_ + selected_width_; j++)
		{
			item_widget = (RenderWidget*)grid_layout_->itemAtPosition(i, j)->widget();
			if (item_widget == nullptr)continue;
			if (!item_widget->isSelected())
			{
				item_widget->setSelected(true);
			}
			item_widget->onRefreshUi();
		}
	}
}

void SplitterWidget::onSetControlerMode(bool status)
{
	is_controler_mode_ = status;
	is_customlayout_mode_ = true;
	if (!is_controler_mode_)
	{
		scroll_area_->setMergeEnable(false);
		scroll_area_->setSplitEnable(false);
	}
}

void SplitterWidget::onResetContent(int count)
{
	if(count > 0 && count >25)
	{
		is_customlayout_mode_ = true;

	}
	else
	{
		current_widgets_count_ = count;
		grid_rows_ = sqrt(count);
		grid_colums_ = grid_rows_;
		is_customlayout_mode_ = false;
	}
	//resetSplitterContent();
}

void SplitterWidget::onResetSplitterContent(int count)
{
	if (count <= 0 || count > 25)
	{
		if (!is_customlayout_mode_)
		{
			is_need_init_ = true;
			is_customlayout_mode_ = true;
			scroll_area_->setMergeEnable();
			scroll_area_->setSplitEnable();
			current_widgets_count_ = -1;
			for (int i = 0; i < grid_rows_; i++)
			{
				for (int j = 0; j < grid_colums_; j++)
				{
					QLayoutItem* item = grid_layout_->itemAtPosition(i, j);
					if (!item)continue;
					int index = grid_layout_->indexOf(item);
					RenderWidget* item_widget = (RenderWidget*)grid_layout_->takeAt(index)->widget();
					if (!item_widget)continue;
					delete item_widget;
					render_widgets_.erase(std::make_pair(i, j));
				}
			}
		}
	}
	else
	{
		if (is_customlayout_mode_)
		{
			is_need_init_ = true;
			scroll_area_->setMergeEnable(false);
			scroll_area_->setSplitEnable(false);
			is_customlayout_mode_ = false;

			for (int i = 0; i < grid_rows_; i++)
			{
				for (int j = 0; j < grid_colums_; j++)
				{
					QLayoutItem* item = grid_layout_->itemAtPosition(i, j);
					if (!item)continue;
					int index = grid_layout_->indexOf(item);
					RenderWidget* item_widget = (RenderWidget*)grid_layout_->takeAt(index)->widget();
					if (!item_widget)continue;
					delete item_widget;
					render_widgets_.erase(std::make_pair(i, j));
				}
			}
			grid_rows_ = grid_colums_ = sqrt(count);
		}
		else
		{
			int colums = sqrt(count);

			int size = render_widgets_.size();

			int wid_count = 0;
			std::map<std::pair<int, int>, RenderWidget*> temp_widgets;
			for (auto it : render_widgets_)
			{
				auto pr = it.first;
				auto wid = it.second;
				//render_widgets_.erase(it.first);

				QLayoutItem* item = grid_layout_->itemAtPosition(pr.first, pr.second);
				if (!item)continue;
				int index = grid_layout_->indexOf(item);
				RenderWidget* item_widget = (RenderWidget*)grid_layout_->takeAt(index)->widget();
				if (!item_widget)continue;
				if (wid_count >= count)
				{
					delete item_widget;
					continue;
				}
				item_widget->setLayoutInfo(wid_count / colums, wid_count % colums);
				temp_widgets.insert({ std::make_pair(wid_count / colums, wid_count % colums), item_widget });
				wid_count++;
			}

			render_widgets_.clear();

			for (auto it : temp_widgets)
			{
				auto pr = it.first;
				auto wid = it.second;
				grid_layout_->addWidget(wid, pr.first, pr.second);
				render_widgets_.insert(it);
			}
			for (int i = wid_count; i < count; i++)
			{
				RenderWidget* render_widget = new RenderWidget(i / colums, i % colums);
				QObject::connect(render_widget, SIGNAL(sigSelected(int, int)), this, SLOT(onRefreshUi(int, int)));
				grid_layout_->addWidget(render_widget, i / colums, i % colums);
				render_widgets_.insert({ std::make_pair(i / colums, i % colums), render_widget });
			}

			current_widgets_count_ = count;
			grid_rows_ = colums;
			grid_colums_ = grid_rows_;
		}

	}

	if (is_need_init_)
	{
		for (int i = 0; i < grid_rows_; i++)
		{
			for (int j = 0; j < grid_colums_; j++)
			{
				QLayoutItem* item = grid_layout_->itemAtPosition(i, j);
				if (!item)continue;
				int index = grid_layout_->indexOf(item);
				RenderWidget* item_widget = (RenderWidget*)grid_layout_->takeAt(index)->widget();
				if (!item_widget)continue;
				delete item_widget;
				render_widgets_.erase(std::make_pair(i, j));
			}
		}

		int scrollbar_height = scroll_area_->getHorizontalScrollbarHeight();
		double scale = (double)(scroll_area_->height() - scrollbar_height) / controler_height_;

		if (is_controler_mode_)
		{
			grid_rows_ = controler_height_ / 100;
			grid_colums_ = controler_width_ / (controler_height_ / grid_rows_);
		}
		else
		{
			if (is_customlayout_mode_)
			{
				grid_rows_ = 16;
				grid_colums_ = this->width() / ((this->height() - scrollbar_height) / grid_rows_);
			}
		}

		for (int i = 0; i < grid_rows_; i++)
		{
			for (int j = 0; j < grid_colums_; j++)
			{
				RenderWidget* wid = new RenderWidget(i, j);
				QObject::connect(wid, SIGNAL(sigSelected(int, int)), this, SLOT(onRefreshUi(int, int)));
				grid_layout_->addWidget(wid, i, j);
				render_widgets_.insert({ std::make_pair(i, j), wid });
			}
		}

		is_need_init_ = false;
	}
	else
	{
		for (int i = 0; i < grid_rows_; i++)
		{
			for (int j = 0; j < grid_colums_; j++)
			{
				auto wid = render_widgets_.find(std::make_pair(i, j));
				if (wid == render_widgets_.end())
				{
					RenderWidget* render_widget = new RenderWidget(i, j);
					QObject::connect(render_widget, SIGNAL(sigSelected(int, int)), this, SLOT(onRefreshUi(int, int)));
					grid_layout_->addWidget(render_widget, i, j);
					render_widgets_.insert({ std::make_pair(i, j), render_widget });
				}
			}
		}
	}

	clearSelection();
}

void SplitterWidget::getRange()
{

	if (start_row_ == -1 || start_colum_ == -1 || selected_width_ == -1 || selected_height_ == -1)
	{
		return;
	}

	QLayoutItem* item = nullptr;
	RenderWidget* item_widget = nullptr;

	int temp_start_row_ = start_row_;
	int temp_start_colum_ = start_colum_;
	int temp_width = selected_width_;
	int temp_height = selected_height_;
	int item_width = 0;
	int item_height = 0;

	for (int i = temp_start_row_; i < temp_start_row_ + temp_height; i++)
	{
		for (int j = temp_start_colum_; j < temp_start_colum_ + temp_width; j++)
		{
			item = grid_layout_->itemAtPosition(i, j);
			if (item == nullptr)continue;
			item_widget = (RenderWidget*)item->widget();
			if (item_widget == nullptr)continue;

			int block_colum;
			int block_row;
			if (item_widget->isCovered())
			{
				block_colum = item_widget->getBlockColum();
				block_row = item_widget->getBlockRow();
			}
			else
			{
				block_colum = item_widget->getColum();
				block_row = item_widget->getRow();
			}

			item = grid_layout_->itemAtPosition(block_row, block_colum);
			if (item == nullptr)continue;
			item_widget = (RenderWidget*)item->widget();
			if (item_widget == nullptr)continue;

			item_width = item_widget->getWidth();
			item_height = item_widget->getHeight();

			if (item_width > 1)
			{
				if (block_colum == temp_start_colum_)
				{
					if ((block_colum + item_width) >= (temp_start_colum_ + temp_width))
					{
						if (end_colum_ < block_colum + item_width)
						{
							end_colum_ = block_colum + item_width;
						}
					}
					else if ((block_colum + item_width) < (temp_start_colum_ + temp_width))
					{

					}
				}
				else if (block_colum > temp_start_colum_)
				{
					if ((block_colum + item_width) >= (temp_start_colum_ + temp_width))
					{
						if (end_colum_ < block_colum + item_width)
						{
							end_colum_ = block_colum + item_width;
						}
					}
					else if ((block_colum + item_width) < (temp_start_colum_ + temp_width))
					{

					}
				}
				else if (block_colum < temp_start_colum_)
				{
					start_colum_ = block_colum;
					if ((block_colum + item_width) >= (temp_start_colum_ + temp_width))
					{
						if (end_colum_ < temp_start_colum_ + item_width)
						{
							end_colum_ = temp_start_colum_ + item_width;
						}
					}
					else if ((block_colum + item_width) < (temp_start_colum_ + temp_width))
					{
						if (end_colum_ < temp_start_colum_ + temp_width)
						{
							end_colum_ = temp_start_colum_ + temp_width;
						}
					}

				}

			}

			if (item_height > 1)
			{
				if (block_row == temp_start_row_)
				{
					if ((block_row + item_height) >= (temp_start_row_ + temp_height))
					{
						if (end_row_ < block_row + item_height)
						{
							end_row_ = block_row + item_height;
						}
					}
				}

				else if (block_row > temp_start_row_)
				{
					if ((block_row + item_height) >= (temp_start_row_ + temp_height))
					{
						if (end_row_ < block_row + item_height)
						{
							end_row_ = block_row + item_height;
						}
					}
					else if ((block_row + item_height) < (temp_start_row_ + temp_height))
					{

					}
				}

				else if (block_row < temp_start_row_)
				{
					start_row_ = block_row;
					if ((block_row + item_height) >= (temp_start_row_ + temp_height))
					{
						if (end_row_ < block_row + item_height)
						{
							end_row_ = block_row + item_height;
						}
					}
					else if ((block_row + item_height) < (temp_start_row_ + temp_height))
					{
						if (end_row_ < temp_start_row_ + temp_height)
						{
							end_row_ = temp_start_row_ + temp_height;
						}
					}
				}

			}

		}

		selected_width_ = end_colum_ - start_colum_;
		selected_height_ = end_row_ - start_row_;
	}
}

void SplitterWidget::setWidgetsLayout(int count)
{
	current_widgets_count_ = count;
	grid_rows_ = sqrt(count);
	is_customlayout_mode_ = false;
}
