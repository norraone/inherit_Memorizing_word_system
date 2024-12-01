// base_repository.cpp
// This file is a placeholder for the base repository implementation.

#include "base_repository.h"
#include <QSqlError>
#include <QString>

// The implementation of BaseRepository is mostly in the header file
// since it's primarily a base class with protected constructor and database management.

// However, we can add some utility functions that might be useful for derived classes

namespace {
    [[maybe_unused]] void checkDatabaseError(const QSqlDatabase& db) {
        if (db.lastError().isValid()) {
            throw std::runtime_error(
                QString("Database error: %1").arg(db.lastError().text()).toStdString()
            );
        }
    }
}
