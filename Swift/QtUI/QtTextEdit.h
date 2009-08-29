#pragma once
#include <QTextEdit>

namespace Swift {
	class QtTextEdit : public QTextEdit {
		Q_OBJECT
	public:
		QtTextEdit(QWidget* parent = 0);
		virtual QSize sizeHint() const;
	signals:
		void returnPressed();
	protected:
		virtual void keyPressEvent(QKeyEvent* event);
	private slots:
		void handleTextChanged();
	};
}
