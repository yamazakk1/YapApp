#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QJsonObject>
#include <QJsonArray>
#include <QCryptographicHash>
#include <QCoreApplication>

/**
 * @brief Менеджер работы с базой данных приложения
 *
 * Реализует singleton для управления всеми операциями с БД:
 * - Подключение/отключение от БД
 * - Работа с пользователями (регистрация, аутентификация)
 * - Управление контактами и сообщениями
 * - Инициализация структуры БД
 *
 * @warning Все методы требуют предварительного подключения к БД (connect())
 */
class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Получает единственный экземпляр класса
     * @return Ссылка на экземпляр DatabaseManager
     */
    static DatabaseManager& instance();

    /**
     * @brief Подключается к базе данных
     * @return true при успешном подключении
     *
     * @throw std::runtime_error при критических ошибках подключения
     */
    bool connect();

    /**
     * @brief Проверяет активное подключение к БД
     * @return true если подключение установлено и валидно
     */
    bool isConnected() const;

    /**
     * @brief Регистрирует нового пользователя
     * @param username Логин пользователя
     * @param password Пароль (будет захеширован)
     * @param email Email пользователя (опционально)
     * @return true при успешной регистрации
     *
     * @note Пароль хранится в виде хеша (sha256)
     */
    bool registerUser(const QString &username, const QString &password, const QString &email = "");

    /**
     * @brief Аутентифицирует пользователя
     * @param username Логин пользователя
     * @param password Пароль
     * @return QJsonObject с данными пользователя или пустой объект при ошибке
     *
     * @code
     * auto authData = dbManager.authenticateUser("user1", "pass123");
     * if (authData.isEmpty()) {
     *     qWarning() << "Authentication failed";
     * }
     * @endcode
     */
    QJsonObject authenticateUser(const QString &username, const QString &password);

    /**
     * @brief Получает данные пользователя по ID
     * @param userId Идентификатор пользователя
     * @return QJsonObject с данными пользователя
     */
    QJsonObject getUserById(int userId);

    /**
     * @brief Записывает мета данные для файла
     * @param user1 ID первого пользователя
     * @param user2 ID второго пользователя
     * @param fileName название и расширение файла
     * @return QJsonObject с id файла для доступа
     */
    QJsonObject setFileMeta(int user1, int user2, const QString &fileName);

    /**
     * @brief Получает данные пользователя по логину
     * @param username Логин пользователя
     * @return QJsonObject с данными пользователя
     */
    QJsonObject getUserByUsername(const QString &username);

    // Работа с сообщениями
    /**
     * @brief Отправляет сообщение
     * @param senderId ID отправителя
     * @param receiverId ID получателя
     * @param content Текст сообщения
     * @return true при успешной отправке
     */
    bool sendMessage(int senderId, int receiverId, const QString &content);

    /**
     * @brief Получает сообщения пользователя
     * @param userId ID пользователя
     * @param limit Максимальное количество сообщений (по умолчанию 50)
     * @return QJsonArray с сообщениями
     */
    QJsonArray getUserMessages(int userId, int limit = 50);

    /**
     * @brief Помечает сообщение как прочитанное
     * @param messageId ID сообщения
     * @return true при успешном обновлении
     */
    bool markMessageAsRead(int messageId);

    /**
     * @brief Инициализирует структуру БД (таблицы)
     *
     * @warning Удаляет существующие данные при повторном вызове!
     */
    void initDatabaseSchema();

    /**
     * @brief Получает список контактов пользователя
     * @param userId ID пользователя
     * @return QJsonArray с контактами
     */
    QJsonArray getUserContacts(int userId);

    /**
     * @brief Добавляет контакт пользователю
     * @param userId ID пользователя
     * @param contactId ID добавляемого контакта
     * @return true при успешном добавлении
     */
    bool addContact(int userId, int contactId);

    /**
     * @brief Получает историю переписки между двумя пользователями
     * @param user1 ID первого пользователя
     * @param user2 ID второго пользователя
     * @param limit Максимальное количество сообщений (по умолчанию 100)
     * @return QJsonArray с сообщениями
     */
    QJsonArray getChatMessages(int user1, int user2, int limit = 100);

private:
    /**
     * @brief Приватный конструктор
     * @param parent Родительский QObject
     */
    explicit DatabaseManager(QObject *parent = nullptr);

    /// Деструктор (автоматически закрывает соединение с БД)
    ~DatabaseManager();

    /**
     * @brief Хеширует пароль
     * @param password Исходный пароль
     * @return Хеш пароля в виде hex-строки
     */
    QString hashPassword(const QString &password);

    QSqlDatabase m_db;               ///< Объект подключения к БД
    static DatabaseManager* m_instance; ///< Указатель на экземпляр singleton
};

#endif // DATABASE_H
