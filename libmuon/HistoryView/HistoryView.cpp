/***************************************************************************
 *   Copyright © 2010 Jonathan Thomas <echidnaman@kubuntu.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of        *
 *   the License or (at your option) version 3 or any later version        *
 *   accepted by the membership of KDE e.V. (or its successor approved     *
 *   by the membership of KDE e.V.), which shall act as a proxy            *
 *   defined in Section 14 of version 3 of the license.                    *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#include "HistoryView.h"

#include <QtGui/QListView>
#include <QtGui/QTreeView>
#include <QtGui/QVBoxLayout>
#include <QStandardItemModel>

#include <KGlobal>
#include <KIcon>
#include <KLocale>
#include <KDebug>

#include <LibQApt/History>

#include "HistoryProxyModel.h"

HistoryView::HistoryView(QWidget *parent)
    : KVBox(parent)
{
    m_history = new QApt::History(this);

    m_historyModel = new QStandardItemModel(this);
    m_historyModel->setColumnCount(1);
    m_historyModel->setHeaderData(0, Qt::Horizontal, i18nc("@title:column", "Date"));
    m_historyView = new QTreeView(this);

    QDateTime weekAgoTime = QDateTime::currentDateTime().addDays(-7);
    foreach (QApt::HistoryItem *item, m_history->historyItems()) {
        QString category;
        QDateTime startDateTime = item->startDate();
        QString formattedTime = KGlobal::locale()->formatTime(startDateTime.time());

        category = startDateTime.toString("MMMM dd");

//         if (startDateTime > weekAgoTime) {
//             //category = KGlobal::locale()->dayPeriodText(startDateTime.time(), KLocale::LongName);
//             category = startDateTime.toString("dddd");
//         } else {
//             category = startDateTime.toString("MMMM dd");
//         }

        QStandardItem *parentItem = 0;

        if (!m_categoryHash.contains(category)) {
            parentItem = new QStandardItem;
            parentItem->setEditable(false);
            parentItem->setText(category);
            parentItem->setData(startDateTime, HistoryProxyModel::HistoryDateRole);

            m_historyModel->appendRow(parentItem);
            m_categoryHash[category] = parentItem;
        } else {
            parentItem = m_categoryHash.value(category);
        }

        foreach (const QString &package, item->installedPackages()) {
            QStandardItem *historyItem = new QStandardItem;
            historyItem->setEditable(false);
            historyItem->setIcon(KIcon("applications-other").pixmap(32,32));

            QString action = i18nc("@status describes a past-tense action", "Installed");
            QString text = i18nc("@item example: muon installed at 16:00", "%1 %2 at %3",
                                 package, action, formattedTime);
            historyItem->setText(text);
            historyItem->setData(startDateTime, HistoryProxyModel::HistoryDateRole);
            historyItem->setData(QApt::Package::ToInstall, HistoryProxyModel::HistoryActionRole);

            parentItem->appendRow(historyItem);
            int parentState = parentItem->data(HistoryProxyModel::HistoryActionRole).toInt();
            parentState |= QApt::Package::ToInstall;
            parentItem->setData(parentState, HistoryProxyModel::HistoryActionRole);
        }

        foreach (const QString &package, item->upgradedPackages()) {
            QStandardItem *historyItem = new QStandardItem;
            historyItem->setEditable(false);
            historyItem->setIcon(KIcon("applications-other").pixmap(32,32));

            QString action = i18nc("@status describes a past-tense action", "Upgraded");
            QString text = i18nc("@item example: muon installed at 16:00", "%1 %2 at %3",
                                 package, action, formattedTime);
            historyItem->setText(text);
            historyItem->setData(startDateTime, HistoryProxyModel::HistoryDateRole);
            historyItem->setData(QApt::Package::ToUpgrade, HistoryProxyModel::HistoryActionRole);

            parentItem->appendRow(historyItem);
            int parentState = parentItem->data(HistoryProxyModel::HistoryActionRole).toInt();
            parentState |= QApt::Package::ToUpgrade;
            parentItem->setData(parentState, HistoryProxyModel::HistoryActionRole);
        }

        foreach (const QString &package, item->downgradedPackages()) {
            QStandardItem *historyItem = new QStandardItem;
            historyItem->setEditable(false);
            historyItem->setIcon(KIcon("applications-other").pixmap(32,32));

            QString action = i18nc("@status describes a past-tense action", "Downgraded");
            QString text = i18nc("@item example: muon installed at 16:00", "%1 %2 at %3",
                                 package, action, formattedTime);
            historyItem->setText(text);
            historyItem->setData(startDateTime, HistoryProxyModel::HistoryDateRole);
            historyItem->setData(QApt::Package::ToDowngrade, HistoryProxyModel::HistoryActionRole);

            parentItem->appendRow(historyItem);
            int parentState = parentItem->data(HistoryProxyModel::HistoryActionRole).toInt();
            parentState |= QApt::Package::ToDowngrade;
            parentItem->setData(parentState, HistoryProxyModel::HistoryActionRole);
        }

        foreach (const QString &package, item->removedPackages()) {
            QStandardItem *historyItem = new QStandardItem;
            historyItem->setEditable(false);
            historyItem->setIcon(KIcon("applications-other").pixmap(32,32));

            QString action = i18nc("@status describes a past-tense action", "Removed");
            QString text = i18nc("@item example: muon installed at 16:00", "%1 %2 at %3",
                                 package, action, formattedTime);
            historyItem->setText(text);
            historyItem->setData(startDateTime, HistoryProxyModel::HistoryDateRole);
            historyItem->setData(QApt::Package::ToRemove, HistoryProxyModel::HistoryActionRole);

            parentItem->appendRow(historyItem);
            int parentState = parentItem->data(HistoryProxyModel::HistoryActionRole).toInt();
            parentState |= QApt::Package::ToRemove;
            parentItem->setData(parentState, HistoryProxyModel::HistoryActionRole);
        }

        foreach (const QString &package, item->purgedPackages()) {
            QStandardItem *historyItem = new QStandardItem;
            historyItem->setEditable(false);
            historyItem->setIcon(KIcon("applications-other").pixmap(32,32));

            QString action = i18nc("@status describes a past-tense action", "Purged");
            QString text = i18nc("@item example: muon installed at 16:00", "%1 %2 at %3",
                                 package, action, formattedTime);
            historyItem->setText(text);
            historyItem->setData(startDateTime, HistoryProxyModel::HistoryDateRole);
            historyItem->setData(QApt::Package::ToPurge, HistoryProxyModel::HistoryActionRole);

            parentItem->appendRow(historyItem);
            int parentState = parentItem->data(HistoryProxyModel::HistoryActionRole).toInt();
            parentState |= QApt::Package::ToRemove;
            parentItem->setData(parentState, HistoryProxyModel::HistoryActionRole);
        }
    }

    m_historyView->setMouseTracking(true);
    m_historyView->setVerticalScrollMode(QListView::ScrollPerPixel);

    m_proxyModel = new HistoryProxyModel(this);
    m_proxyModel->setSourceModel(m_historyModel);
    m_proxyModel->sort(0);

    m_historyView->setModel(m_proxyModel);
}

HistoryView::~HistoryView()
{
}

#include "HistoryView.moc"
