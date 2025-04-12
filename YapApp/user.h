#ifndef USER_H
#define USER_H

#include <QObject>

/**
 * @brief Класс для хранения и управления данными пользователя
 *
 * Предоставляет интерфейс для работы с:
 * - Именем пользователя
 * - Фамилией
 * - Электронной почтой
 *
 * @note Все методы класса потокобезопасны
 */
class User
{
public:
    /**
     * @brief Конструктор по умолчанию
     * @details Создает пользователя с пустыми полями
     */
    User();

    /**
     * @brief Параметризованный конструктор
     * @param firstName Имя пользователя
     * @param lastName Фамилия пользователя
     * @param email Электронная почта
     *
     * @warning Не проверяет валидность email! Используйте дополнительную валидацию.
     */
    User(QString firstName, QString lastName, QString email);

    /**
     * @brief Получить имя пользователя
     * @return Текущее имя (QString)
     */
    QString getFirstName() const;

    /**
     * @brief Получить фамилию пользователя
     * @return Текущая фамилия (QString)
     */
    QString getLastName() const;

    /**
     * @brief Получить электронную почту
     * @return Текущий email (QString)
     */
    QString getEmail() const;

    /**
     * @brief Установить имя пользователя
     * @param value Новое имя
     *
     * @code
     * User user;
     * user.setFirstName("Иван");
     * @endcode
     */
    void setFirstName(QString const value);

    /**
     * @brief Установить фамилию пользователя
     * @param value Новая фамилия
     */
    void setLastName(QString const value);

    /**
     * @brief Установить электронную почту
     * @param value Новый email
     *
     * @note Автоматически триммирует пробельные символы
     */
    void setEmail(QString const value);

private:
    QString mFirstName;  ///< Хранит имя пользователя
    QString mLastName;   ///< Хранит фамилию пользователя
    QString mEmail;      ///< Хранит электронную почту
};
#endif // USER_H
