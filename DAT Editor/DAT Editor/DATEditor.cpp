#include "stdafx.h"
#include "DATEditor.h"
#include "AboutDlg.h"

DATEditor::DATEditor(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	showMaximized();

	connect(ui.action_Open, SIGNAL(triggered()), this, SLOT(on_Action_Open()));
	connect(ui.action_Save, SIGNAL(triggered()), this, SLOT(on_Action_Save()));
	connect(ui.action_SaveAs, SIGNAL(triggered()), this, SLOT(on_Action_SaveAs()));
	connect(ui.action_Close, SIGNAL(triggered()), this, SLOT(on_Action_Close()));
	connect(ui.action_Quit, SIGNAL(triggered()), this, SLOT(on_Action_Quit()));
	connect(ui.action_Import, SIGNAL(triggered()), this, SLOT(on_Action_Import()));
	connect(ui.action_Export, SIGNAL(triggered()), this, SLOT(on_Action_Export()));

	connect(ui.action_MulLine, SIGNAL(triggered()), this, SLOT(on_Action_MLineEdit()));
	connect(ui.action_Find, SIGNAL(triggered()), this, SLOT(on_Action_Find()));
	connect(ui.action_FindNext, SIGNAL(triggered()), this, SLOT(on_Action_FindNext()));
	connect(ui.action_Replace, SIGNAL(triggered()), this, SLOT(on_Action_Replace()));
	connect(ui.action_ReplaceNext, SIGNAL(triggered()), this, SLOT(on_Action_ReplaceNext()));
	connect(ui.action_ReplaceAll, SIGNAL(triggered()), this, SLOT(on_Action_ReplaceAll()));
	connect(ui.action_Row, SIGNAL(triggered()), this, SLOT(on_Action_JumpToRow()));
	connect(ui.action_Column, SIGNAL(triggered()), this, SLOT(on_Action_JumpToColumn()));

	connect(ui.action_About, SIGNAL(triggered()), this, SLOT(on_Action_About()));

	statusText = new QLabel(this);
	ui.statusBar->addWidget(statusText);
	statusText->setText(QString("����"));
	statusText->setAlignment(Qt::AlignLeft);
	statusInfo = new QLabel(this);
	ui.statusBar->addPermanentWidget(statusInfo);
	statusInfo->setText(QString("�����������л����ṩͼ��"));
}

void DATEditor::on_Action_Open() {
	if (!curName.isEmpty())
		on_Action_Close();

	if (curName.isEmpty()) {
		QString fileName = QFileDialog::getOpenFileName(this, "ѡ��DAT�ļ�", "", "300 DAT �ļ� (*.dat)");
		if (!fileName.isEmpty()) {
			m_data = new DATData();
			try {
				m_data->loadFromFile(QFile(fileName));
			} catch (QString msg) {
				QMessageBox::critical(this, "����", msg);
				delete m_data;
				m_data = nullptr;
			}

			if (m_data != nullptr) {
				bool ok;
				QString dat = QInputDialog::getItem(this, "ѡ��dat����", "DAT", m_supdats, 0, false, &ok);
				if (!ok) {
					delete m_data;
					m_data = nullptr;
				} else {
					m_data->setDatType(m_indexmethod[m_supdats.indexOf(dat)]);
					if (!m_data->canPatch()) {
						QMessageBox::critical(this, "����", "��DAT�ļ��޷���������");
						delete m_data;
						m_data = nullptr;
					}
				}
			}

			if (m_data != nullptr) {
				m_orindata = new DATData(*m_data);
				curPath = fileName;
				curName = QFileInfo(curPath).fileName();
				setupTable();
			}
		}
	}
}

void DATEditor::on_Action_Save() {
	if (!curName.isEmpty()) {
		saveScene(curPath);
	}
}

void DATEditor::on_Action_SaveAs() {
	if (!curName.isEmpty()) {
		QString saveFile = QFileDialog::getSaveFileName(this, "ѡ�񱣴�λ��", curPath, "300 DAT �ļ� (*.dat)");
		if (!saveFile.isEmpty()) {
			if (saveScene(saveFile)) {
				curPath = saveFile;
				curName = QFileInfo(curPath).fileName();
			}
		}
	}
}

void DATEditor::on_Action_Import() {
	if (curName.isEmpty())
		on_Action_Open();

	delete m_data;
	m_data = new DATData(*m_orindata);
	if (!curName.isEmpty()) {
		QString openFile = QFileDialog::getOpenFileName(this, "ѡ�񲹶�λ��", curPath, "300 DAT �����ļ� (*.dpk)");
		if (!openFile.isEmpty()) {
			try {
				m_data->importFromFile(m_orindata, QFile(openFile));
			} catch (QString msg) {
				QMessageBox::critical(this, "����", msg);
				return;
			}
			clearTable();
			setupTable();
		}
	}
}

void DATEditor::on_Action_Export() {
	if (!curName.isEmpty()) {
		QFileInfo info(curPath);
		QString savePath = info.absolutePath() + "/" + info.baseName() + ".dpk";
		QString saveFile = QFileDialog::getSaveFileName(this, "ѡ�񱣴�λ��", savePath, "300 DAT �����ļ� (*.dpk)");
		if (!saveFile.isEmpty()) {
			try {
				m_data->exportToFile(m_orindata, QFile(saveFile));
			} catch (QString msg) {
				QMessageBox::critical(this, "����", msg);
				return;
			}
		}
	}
}

void DATEditor::on_Action_Close() {
	if (!curName.isEmpty()) {
		switch (QMessageBox::warning(this, "�ر�", "�Ƿ񱣴��ļ���", QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel)) {
			case QMessageBox::Yes:
				if (!saveScene(curPath)) return;
			case QMessageBox::No:
				clearTable();
				curPath = curName = "";
				delete m_data;
				m_data = nullptr;
				delete m_orindata;
			default: return;
		}
	}
}

void DATEditor::on_Action_Quit() {
	on_Action_Close();
	if (curName.isEmpty())
		close();
}

void DATEditor::setupTable() {
	tableModel = new QStandardItemModel();
	tableModel->setColumnCount(m_data->getColumnCount());
	tableModel->setRowCount(m_data->getRowCount());
	for (int i = 0; i < m_data->getColumnCount(); i++) {
		int type = m_data->getType(i);
		QString title = QString::number(i + 1) + ": " + (type == QVariant::Int ? "����" :
			type == QVariant::Double ? "������" : type == QVariant::String ? "�ַ���" :
			type == QVariant::Invalid ? "������" : "δ֪");
		tableModel->setHeaderData(i, Qt::Horizontal, title);
	}
	for (int i = 0; i < m_data->getRowCount(); i++) {
		for (int j = 0; j < m_data->getColumnCount(); j++) {
			QVariant data = m_data->getData(i, j);
			QStandardItem *item = new QStandardItem(data.toString());
			if (m_data->getType(j) == QVariant::Invalid)
				item->setFlags(item->flags() & (~Qt::ItemIsEditable));
			tableModel->setItem(i, j, item);
		}
	}
	ui.tableView->setModel(tableModel);
	ui.tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	for (int i = 0; i < m_data->getRowCount(); i++)
		ui.tableView->setRowHeight(i, 24);
	ui.tableView->resizeColumnsToContents();
	for (int i = 0; i < m_data->getColumnCount(); i++) {
		ui.tableView->setColumnWidth(i, min(width() / 4, ui.tableView->columnWidth(i)));
	}
	connect(tableModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(on_Data_Changed(QStandardItem*)));
	statusText->setText(QString("������ϣ��� %1 �У��� %2 �У�%3 ��Ԫ��").arg(m_data->getRowCount()).arg(m_data->getColumnCount())
		.arg(m_data->getRowCount() * m_data->getColumnCount()));
}

void DATEditor::clearTable() {
	delete tableModel;
	searchStr = "";
	statusText->clear();
	statusText->setText(QString("����"));
}

bool DATEditor::saveScene(QString path) {
	try {
		m_data->saveToFile(QFile(path));
	} catch (QString msg) {
		QMessageBox::critical(this, "����", msg);
		return false;
	}

	statusText->setText(QString("�������"));
	return true;
}

void DATEditor::on_Data_Changed(QStandardItem *item) {
	switch (m_data->getType(item->column())) {
		case QVariant::Int:
			m_data->setData(item->row(), item->column(), item->text().toInt());
			break;
		case QVariant::Double:
			m_data->setData(item->row(), item->column(), item->text().toDouble());
			break;
		case QVariant::String:
			m_data->setData(item->row(), item->column(), item->text());
			break;
		default: QMessageBox::critical(this, "����", "���ɱ༭�˴�");
	}
	item->setText(m_data->getData(item->row(), item->column()).toString());
}

void DATEditor::on_Action_MLineEdit() {
	QModelIndex index = ui.tableView->currentIndex();
	if (index.isValid()) {
		if (m_data->getType(index.column()) == QVariant::String) {
			bool ok;
			QString res = QInputDialog::getMultiLineText(this, "�༭�ַ���", "",
				m_data->getData(index.row(), index.column()).toString(), &ok, Qt::WindowMaximizeButtonHint);
			if (ok) {
				tableModel->setData(index, res);
			}
		}
	}
}

void DATEditor::on_Action_Find() {
	if (!curName.isEmpty()) {
		bool ok;
		QString res = QInputDialog::getText(this, "�����ַ���", "", QLineEdit::Normal, searchStr, &ok);
		if (ok) {
			if (res.isEmpty())
				QMessageBox::critical(this, "����", "�����ַ�������Ϊ��");
			else {
				searchStr = res;
				if (!searchNext())
					QMessageBox::information(this, "����", "�ѵ�������ĩβ");
			}
		}
	}
}

void DATEditor::on_Action_FindNext() {
	if (!curName.isEmpty()) {
		if (!searchStr.isEmpty())
			if (!searchNext())
				QMessageBox::information(this, "����", "�ѵ�������ĩβ");
	}
}

void DATEditor::on_Action_JumpToRow() {
	if (!curName.isEmpty()) {
		bool ok;
		QModelIndex index = ui.tableView->currentIndex();
		int row = QInputDialog::getInt(this, "��ת����", "", index.row(),
			1, m_data->getRowCount(), 1, &ok) - 1;
		if (ok) {
			index = tableModel->index(row, index.column());
			ui.tableView->setCurrentIndex(index);
			ui.tableView->scrollTo(index);
		}
	}
}

void DATEditor::on_Action_JumpToColumn() {
	if (!curName.isEmpty()) {
		bool ok;
		QModelIndex index = ui.tableView->currentIndex();
		int col = QInputDialog::getInt(this, "��ת����", "", index.column(),
			1, m_data->getColumnCount(), 1, &ok) - 1;
		if (ok) {
			index = tableModel->index(index.row(), col);
			ui.tableView->setCurrentIndex(index);
			ui.tableView->scrollTo(index);
		}
	}
}

bool DATEditor::searchNext() {
	QModelIndex index = ui.tableView->currentIndex();
	int row = index.row(), col = index.column() + 1;
	while (row < m_data->getRowCount()) {
		while (col < m_data->getColumnCount()) {
			if (m_data->getType(col) != QVariant::Invalid) {
				QString data = tableModel->item(row, col)->text();
				if (data.contains(searchStr)) {
					index = tableModel->index(row, col);
					ui.tableView->setCurrentIndex(index);
					ui.tableView->scrollTo(index);
					return true;
				}
			}
			col++;
		}
		row++;
		col = 0;
	}
	return false;
}

void DATEditor::on_Action_Replace() {
	if (!curName.isEmpty()) {
		bool ok;
		QString res = QInputDialog::getText(this, "�����ַ���", "", QLineEdit::Normal, searchStr, &ok);
		if (!ok) return;
		if (res.isEmpty()) {
			QMessageBox::critical(this, "����", "�����ַ�������Ϊ��");
			return;
		}
		searchStr = res;
		if (!searchNext()) {
			QMessageBox::information(this, "����", "�ѵ�������ĩβ");
			return;
		}
		res = QInputDialog::getText(this, "�滻Ϊ", "", QLineEdit::Normal, replaceStr, &ok);
		if (!ok) return;
		replaceStr = res;
		replaceCurrent();
		if (!searchNext())
			QMessageBox::information(this, "����", "�ѵ�������ĩβ");
	}
}

void DATEditor::on_Action_ReplaceNext() {
	if (!curName.isEmpty()) {
		if (!searchStr.isEmpty()) {
			replaceCurrent();
			if (!searchNext())
				QMessageBox::information(this, "����", "�ѵ�������ĩβ");
		}
	}
}

void DATEditor::on_Action_ReplaceAll() {
	if (!curName.isEmpty()) {
		bool ok;
		QString res = QInputDialog::getText(this, "�����ַ���", "", QLineEdit::Normal, searchStr, &ok);
		if (!ok) return;
		if (res.isEmpty()) {
			QMessageBox::critical(this, "����", "�����ַ�������Ϊ��");
			return;
		}
		searchStr = res;
		replaceStr = QInputDialog::getText(this, "�滻Ϊ", "", QLineEdit::Normal, replaceStr, &ok);
		if (!ok) return;
		int cnt;
		ui.tableView->setCurrentIndex(tableModel->index(0, 0));
		for (cnt = 0; searchNext(); cnt++) {
			replaceCurrent();
		}
		QMessageBox::information(this, "�滻ȫ��", QString("������ɣ����滻%1��").arg(cnt));
	}
}

void DATEditor::replaceCurrent() {
	QModelIndex index = ui.tableView->currentIndex();
	int row = index.row(), col = index.column();
	QString str = tableModel->item(row, col)->text();
	str.replace(searchStr, replaceStr);
	tableModel->item(row, col)->setText(str);
}

void DATEditor::on_Action_About() {
//	QMessageBox::information(this, "����", "300 Ӣ�� DAT �༭�� V 1.0\n\t����By Gamepiaynmo");
	AboutDlg(this).exec();
}