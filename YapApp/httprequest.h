#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <QString>
#include <QMap>

/**
 * @brief Класс для представления и обработки HTTP-запросов
 *
 * Содержит разобранные компоненты HTTP-запроса:
 * - Метод (GET/POST/etc)
 * - Путь
 * - Заголовки
 * - Query-параметры
 * - Тело запроса
 * - Статус валидности
 */
class HttpRequest
{
public:
    QString method;      ///< HTTP-метод в верхнем регистре (например, "GET", "POST")
    QString path;       ///< Путь запроса без query-параметров (например, "/api/users")
    QMap<QString, QString> headers;     ///< Заголовки запроса (ключ в нижнем регистре)
    QMap<QString, QString> queryParams; ///< Параметры URL (после '?')
    QByteArray body;     ///< Тело запроса (для POST/PUT)
    bool valid = true;   ///< Флаг корректности разбора запроса

    /**
     * @brief Получает значение заголовка по имени
     * @param name Название заголовка (регистронезависимое)
     * @return Значение заголовка или пустую строку
     *
     * @code
     * HttpRequest request;
     * QString contentType = request.header("content-type");
     * @endcode
     */
    QString header(const QString& name) const;

    /**
     * @brief Получает query-параметр по имени
     * @param name Ключ параметра
     * @return Значение параметра или пустую строку
     */
    QString query(const QString& name) const;

    /**
     * @brief Проверяет валидность запроса
     * @return true если запрос был успешно разобран
     */
    bool isValid() const;

    /**
     * @brief Парсит сырой HTTP-запрос
     * @param rawRequest Сырые данные запроса (как пришли от клиента)
     * @return Разобранный HttpRequest объект
     *
     * @throw std::runtime_error при критических ошибках парсинга
     * @note Устанавливает valid=false при нефатальных ошибках
     *
     * @code
     * QByteArray data = socket->readAll();
     * HttpRequest request = HttpRequest::parse(data);
     * if (!request.isValid()) {
     *     sendErrorResponse(400);
     * }
     * @endcode
     */
    static HttpRequest parse(const QByteArray& rawRequest);
};
#endif // HTTPREQUEST_H
