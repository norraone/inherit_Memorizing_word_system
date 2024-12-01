#ifndef BASE_REPOSITORY_H
#define BASE_REPOSITORY_H

#include <QSqlDatabase>
#include <memory>
#include <stdexcept>

class BaseRepository {
protected:
    QSqlDatabase& db;

    BaseRepository() : db(getDatabase()) {}

    static QSqlDatabase& getDatabase() {
        static QSqlDatabase database = QSqlDatabase::database();
        if (!database.isValid()) {
            throw std::runtime_error("Database connection not initialized");
        }
        return database;
    }

public:
    virtual ~BaseRepository() = default;
};

#endif // BASE_REPOSITORY_H
