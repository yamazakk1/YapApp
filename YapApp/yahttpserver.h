#ifndef YAHTTPSERVER_H
#define YAHTTPSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QUuid>
#include <QJsonObject>
#include <QJsonDocument>
#include "httprequest.h"

/**
 * @brief TCP-сервер с HTTP-обработчиком входящих соединений
 *
 * Наследует QTcpServer и добавляет функциональность:
 * - Автоматический парсинг HTTP-запросов
 * - Генерацию уникальных токенов для сессий
 * - Сигнальную систему для обработки событий
 *
 * @note Для использования создайте экземпляр и подключите сигналы к слотам обработчиков
 */
class YaHttpServer : public QTcpServer
{
    Q_OBJECT

public:
    /**
     * @brief Конструктор HTTP-сервера
     * @param parent Родительский QObject (для управления памятью)
     */
    explicit YaHttpServer(QObject* parent = nullptr);

    /**
     * @brief Генерирует уникальный токен сессии
     * @return 32-байтовый hex-токен в формате QString
     *
     * @code
     * QString token = YaHttpServer::generateToken();
     * qDebug() << "Session token:" << token;
     * @endcode
     *
     * @warning Не криптографически безопасен! Только для идентификации сессий.
     */
    static QString generateToken();

protected:
    /**
     * @brief Обработчик новых TCP-соединений (переопределение QTcpServer)
     * @param socketDescriptor Дескриптор входящего сокета
     *
     * @internal
     * 1. Создает QTcpSocket для дескриптора
     * 2. Настраивает парсинг HTTP-заголовков
     * 3. Подключает сигналы readyRead/disconnected
     */
    void incomingConnection(qintptr socketDescriptor) override;

signals:
    /**
     * @brief Сигнал нового HTTP-запроса
     * @param socket Ссылка на сокет клиента
     * @param request Распарсенный HTTP-запрос (метод, заголовки, тело)
     *
     * @note Сокет остается открытым - необходимо самостоятельно отправлять ответ
     */
    void newRequest(QTcpSocket& socket, HttpRequest& request);

    /**
     * @brief Сигнал отключения клиента
     * @param socket Ссылка на сокет отключившегося клиента
     *
     * @warning Не используйте сокет после этого сигнала!
     */
    void disconnect(QTcpSocket& socket);
};

#endif // YAHTTPSERVER_H
