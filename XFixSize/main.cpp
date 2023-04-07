#include "JumpX.h"
#pragma execution_character_set("utf-8")

QTextStream qout(stdout, QIODevice::WriteOnly);

bool process(QFile& input, QFile& output) {
	XScene* scene = new XScene();
	try {
		qout << QString("���������ļ�...") << endl << endl;

		scene->loadFromFile(input);
		while (scene->haveWarning())
			qout << scene->nextWarning() << endl;

		qout << QString("�����ļ����") << endl;
		qout << QString("X�ļ��汾 %1\n������֡�� %2\n%3 ����ͼ\n%4 ������\n%5 ������\n%6 ������\n%7 ��β��\n%8 ������\n%9 ������")
			.arg(scene->m_head.fileVersion).arg(scene->m_head.frameCount).arg(scene->m_head.ntex).arg(scene->m_head.nmtl)
			.arg(scene->m_head.ngeo).arg(scene->m_head.nbon).arg(scene->m_head.nrib).arg(scene->m_head.nprt).arg(scene->m_head.nact) << endl;
		qout << endl;

		qout << QString("�ڲ����ݴ�С��%1 %2 10M��%3 %4 10M").arg(scene->m_head.dataSize).arg(scene->m_head.dataSize >= g_limit ? ">" : "<")
			.arg(scene->m_head.indexSize).arg(scene->m_head.indexSize >= g_limit ? ">" : "<") << endl;
		bool needFix = scene->m_head.dataSize >= g_limit || scene->m_head.indexSize >= g_limit;
		qout << QString(needFix ? "��Ҫ�޸�" : "�����޸�") << endl << endl;

		if (needFix) {
			qout << QString("��������ļ�����") << output.fileName() << endl << endl;

			scene->saveToFile(output);
			while (scene->haveWarning())
				qout << scene->nextWarning() << endl;

			qout << QString("�޸������ݴ�С��%1 %2 10M��%3 %4 10M").arg(scene->m_head.dataSize).arg(scene->m_head.dataSize >= g_limit ? ">" : "<")
				.arg(scene->m_head.indexSize).arg(scene->m_head.indexSize >= g_limit ? ">" : "<") << endl;
			if (scene->m_head.dataSize >= g_limit || scene->m_head.indexSize >= g_limit)
				throw QString("�ļ�̫���޷��޸������ֶ��Ż�ģ��");
			qout << QString("�޸��ɹ�") << endl << endl;

			qout << QString("�����ļ����") << endl;
		}
	}
	catch (QString msg) {
		qout << QString("����") << msg << endl;
	}

	delete scene;
	getchar();
	return true;
}

void seh_handler(unsigned int u, EXCEPTION_POINTERS* pExp) {
	throw QString("SEH �쳣 ") + QString::number(pExp->ExceptionRecord->ExceptionCode);
}

int main(int argc, char *argv[]) {
	_set_se_translator(seh_handler);

    QCoreApplication a(argc, argv);
	qout << QString("JumpX �ļ���С�޸� V1.1") << endl;
	qout << QString("���ߣ��ڤ����") << endl << endl;

	if (argc < 2) {
		qout << QString("ʹ�÷�����XFixSize �����ļ� [����ļ�]") << endl;
		qout << QString("����ֱ�ӽ�X�ļ��϶�����exeͼ����") << endl;
		getchar();
		return 0;
	}

	QString input = argv[1];
	QString output;
	if (argc < 3) {
		QFileInfo info(input);
		output = info.absoluteDir().filePath(info.baseName() + "_fixed.x");
	} else output = argv[2];
	process(QFile(input), QFile(output));

    return 0;
}
