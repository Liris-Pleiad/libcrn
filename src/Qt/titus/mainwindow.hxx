/*!
 * \file mainwindow.hxx
 * \brief mainwindow file.
 * \author Martial TOLA, CNRS-Lyon, LIRIS UMR 5205
 * \date 2016
 */
#ifndef HEADER_mainwindow
#define HEADER_mainwindow

#include <QMainWindow>

/*!
 * \class mainwindow
 * \brief mainwindow class.
 */
class mainwindow : public QMainWindow
{
    Q_OBJECT

    public:
		/*!
		 * \brief Constructor.
		 *
		 * \param parent : parent window.
		 */
        mainwindow(QMainWindow *parent = 0);
		/*!
		 * \brief Destructor.
		 */
		~mainwindow();

	protected:

    private slots:

	private:
};

#endif
