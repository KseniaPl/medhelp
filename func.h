#ifndef FUNC
#define FUNC

#include <QTime>
#include <QCoreApplication>
#include <sstream>

//доп.функции
namespace func
{
//перевод int,double и др. в string
template < typename T > std::string to_string( const T& n )
{
    std::ostringstream stm ;
    stm << n ;
    return stm.str() ;
}

//безопасная пауза в Qt без блокирования GUI
static void safeDelay( int millisecondsToWait )
{
    QTime dieTime = QTime::currentTime().addMSecs( millisecondsToWait );
    while( QTime::currentTime() < dieTime )
    {
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
    }
}

}

#endif // FUNC
