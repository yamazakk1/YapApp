#ifndef YASERVER_H
#define YASERVER_H

#include <QObject>
#include <QTcpSocket>
#include <QFileInfo>
#include <QDir>
#include "yahttpserver.h"
#include "database.h"

/**
 * @brief Singleton-класс HTTP-сервера для обработки входящих запросов.
 *
 * Класс реализует TCP-сервер с HTTP-интерфейсом. Поддерживает:
 * - Обработку входящих соединений
 * - Парсинг HTTP-запросов
 * - Управление жизненным циклом соединений
 *
 * @warning Класс является singleton. Для доступа используйте getInstance().
 * @see YaHttpServer
 */
class YaServer : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Получает единственный экземпляр класса
     * @return Ссылка на экземпляр YaServer
     *
     * @code
     * auto& server = YaServer::getInstance();
     * server.startServer(8080);
     * @endcode
     */
    static YaServer& getInstance();

    /**
     * @brief Запускает сервер на указанном порту
     * @param port Порт для прослушивания (1-65535)
     * @throw std::runtime_error Если сервер не может стартовать
     */
    void startServer(int port);

    YaServer(const YaServer&) = delete;            ///< Запрет копирования
    YaServer& operator=(const YaServer&) = delete; ///< Запрет присваивания

private slots:
    /**
     * @brief Обрабатывает новый HTTP-запрос
     * @param socket Сокет клиента
     * @param request Распарсенный HTTP-запрос
     */
    void newRequest(QTcpSocket &socket, HttpRequest &request);

    /**
     * @brief Обрабатывает отключение клиента
     * @param socket Сокет отключившегося клиента
     */
    void disconnect(QTcpSocket &socket);

private:
    /**
     * @brief Приватный конструктор
     * @param parent Родительский QObject
     *
     * @note Используйте getInstance() для создания объекта
     */
    explicit YaServer(QObject *parent = nullptr);

    /// Деструктор (автоматически останавливает сервер)
    ~YaServer();

    YaHttpServer *httpServer = nullptr; ///< Указатель на реализацию HTTP-сервера
};

#endif // YASERVER_H
