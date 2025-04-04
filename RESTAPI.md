
# YAChat REST API

## 📖 Описание

YAChat Server предоставляет простой REST API, основанный на JSON и самописном HTTP-подобном протоколе. Несмотря на отсутствие стандартной HTTP-библиотеки, интерфейс организован по REST-принципам.

---

## 🧭 Список методов

### 📝 `POST /register`

**Описание:** Регистрация нового пользователя.

**Тело запроса:**
```json
{
  "username": "alice",
  "password": "secret",
  "email": "alice@example.com"
}
```

**Ответ:**
```json
{
  "type": "register",
  "success": true
}
```

**Ошибка:**
```json
{
  "type": "register",
  "success": false,
  "error": "Registration failed"
}
```

---

### 🔑 `POST /login`

**Описание:** Вход пользователя по email и паролю.

**Тело запроса:**
```json
{
  "email": "alice@example.com",
  "password": "secret"
}
```

**Ответ (успех):**
```json
{
  "type": "login",
  "id": 1,
  "username": "alice",
  "email": "alice@example.com",
  "token": "abc123xyz"
}
```

**Ошибка:**
```json
{
  "type": "login",
  "error": "Authentication failed"
}
```

---

### 👥 `GET /contacts?user_id=1`

**Описание:** Получить список контактов пользователя.

**Ответ:**
```json
{
  "type": "contacts",
  "contacts": [
    {
      "id": 2,
      "username": "bob",
      "email": "bob@example.com"
    }
  ]
}
```

---

### ➕ `POST /contacts`

**Описание:** Добавить контакт в список пользователя.

**Тело запроса:**
```json
{
  "user_id": 1,
  "contact_id": 2
}
```

**Ответ:**
```json
{
  "type": "contacts_add",
  "success": true
}
```

---

### ✉️ `POST /messages`

**Описание:** Отправить сообщение другому пользователю.

**Тело запроса:**
```json
{
  "sender_id": 1,
  "receiver_id": 2,
  "content": "Привет!"
}
```

**Ответ:**
```json
{
  "type": "messages_send",
  "success": true
}
```

---

### 📩 `GET /messages?user1_id=1&user2_id=2&limit=100`

**Описание:** Получить историю сообщений между двумя пользователями.

**Ответ:**
```json
{
  "type": "messages",
  "messages": [
    {
      "id": 1,
      "sender_id": 1,
      "receiver_id": 2,
      "content": "Привет!",
      "timestamp": "2025-04-04T12:00:00"
    }
  ]
}
```

---

### 🔍 `GET /users?username=alice`

или

### 🔍 `GET /users?id=1`

**Описание:** Найти пользователя по имени или ID.

**Ответ (успех):**
```json
{
  "type": "user_search",
  "success": true,
  "id": 1,
  "username": "alice",
  "email": "alice@example.com"
}
```

**Ответ (не найден):**
```json
{
  "type": "user_search",
  "success": false
}
```

---

## ⚙️ Технические детали

- Все запросы и ответы — в формате JSON.
- Заголовок `Content-Type: application/json` обязателен.
- Возвращается всегда `HTTP/1.1 200 OK`, даже при ошибке (ошибки указываются в JSON).
- Сервер не использует стандартный HTTP-движок — разбор заголовков реализован вручную.
