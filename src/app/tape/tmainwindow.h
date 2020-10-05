/************************************************************************
 **
 **  @file   tmainwindow.h
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   10 7, 2015
 **
 **  @brief
 **  @copyright
 **  This source code is part of the Valentina project, a pattern making
 **  program, whose allow create and modeling patterns of clothing.
 **  Copyright (C) 2015 Valentina project
 **  <https://gitlab.com/smart-pattern/valentina> All Rights Reserved.
 **
 **  Valentina is free software: you can redistribute it and/or modify
 **  it under the terms of the GNU General Public License as published by
 **  the Free Software Foundation, either version 3 of the License, or
 **  (at your option) any later version.
 **
 **  Valentina is distributed in the hope that it will be useful,
 **  but WITHOUT ANY WARRANTY; without even the implied warranty of
 **  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **  GNU General Public License for more details.
 **
 **  You should have received a copy of the GNU General Public License
 **  along with Valentina.  If not, see <http://www.gnu.org/licenses/>.
 **
 *************************************************************************/

#ifndef TMAINWINDOW_H
#define TMAINWINDOW_H

#include <QTableWidget>

#include "../vmisc/def.h"
#include "../vmisc/vlockguard.h"
#include "../vformat/vmeasurements.h"
#include "../vmisc/vtablesearch.h"
#include "../vwidgets/vabstractmainwindow.h"

namespace Ui
{
    class TMainWindow;
} // namespace Ui

class QLabel;
class QxtCsvModel;
class VMeasurement;

class TMainWindow : public VAbstractMainWindow
{
    Q_OBJECT

public:
    explicit TMainWindow(QWidget *parent = nullptr);
    virtual ~TMainWindow() override;

    QString CurrentFile() const;

    void RetranslateTable();

    bool SetDimensionABase(int base);
    bool SetDimensionBBase(int base);
    bool SetDimensionCBase(int base);
    void SetPUnit(Unit unit);

    bool LoadFile(const QString &path);

public slots:
    virtual void ShowToolTip(const QString &toolTip) override;

protected:
    virtual void closeEvent(QCloseEvent *event) override;
    virtual void changeEvent(QEvent* event) override;
    virtual bool eventFilter(QObject *object, QEvent *event) override;
    virtual void ExportToCSVData(const QString &fileName, bool withHeader, int mib,
                                 const QChar &separator) final;
    virtual QStringList RecentFileList() const override;

private slots:
    void FileNew();
    void OpenIndividual();
    void OpenMultisize();
    void OpenTemplate();
    void CreateFromExisting();
    void Preferences();
    void ToolBarStyles();

    bool FileSave();
    bool FileSaveAs();
    void AboutToShowWindowMenu();
    void ShowWindow() const;
    void ImportDataFromCSV();

#if defined(Q_OS_MAC)
    void AboutToShowDockMenu();
    void OpenAt(QAction *where);
#endif //defined(Q_OS_MAC)

    void SaveCustomerName();
    void SaveEmail();
    void SaveGender(int index);
    void SaveBirthDate(const QDate & date);
    void SaveNotes();
    void SavePMSystem(int index);

    void Remove();
    void MoveTop();
    void MoveUp();
    void MoveDown();
    void MoveBottom();
    void Fx();

    void AddCustom();
    void AddKnown();
    void ImportFromPattern();

    void DimensionABaseChanged();
    void DimensionBBaseChanged();
    void DimensionCBaseChanged();

    void GradationChanged();

    void ShowMData();

    void DeployFormula();

    void SaveMName(const QString &text);
    void SaveMValue();
    void SaveMBaseValue(double value);
    void SaveMShiftA(double value);
    void SaveMShiftB(double value);
    void SaveMShiftC(double value);
    void SaveMCorrectionValue(double value);
    void SaveMDescription();
    void SaveMFullName();
    void SaveMUnits();

    void FullCircumferenceChanged(bool checked);

    void ExportToIndividual();

private:
    Q_DISABLE_COPY(TMainWindow)
    Ui::TMainWindow *ui;
    VMeasurements   *m{nullptr};
    VContainer      *data{nullptr};
    Unit             mUnit{Unit::Cm};
    Unit             pUnit{Unit::Cm};
    MeasurementsType mType{MeasurementsType::Individual};
    int              currentDimensionA{0};
    int              currentDimensionB{0};
    int              currentDimensionC{0};
    QString          curFile{};
    QComboBox       *gradationDimensionA{nullptr};
    QComboBox       *gradationDimensionB{nullptr};
    QComboBox       *gradationDimensionC{nullptr};
    QComboBox       *comboBoxUnits{nullptr};
    int              formulaBaseHeight;
    QSharedPointer<VLockGuard<char>> lock{nullptr};
    QSharedPointer<VTableSearch> search{};
    QLabel *labelGradationDimensionA{nullptr};
    QLabel *labelGradationDimensionB{nullptr};
    QLabel *labelGradationDimensionC{nullptr};
    QLabel *labelPatternUnit{nullptr};
    bool isInitialized{false};
    bool mIsReadOnly{false};
    QAction *actionFullCircumference{nullptr};
    QTimer *gradation;

    QVector<QObject *> hackedWidgets{};

    void SetupMenu();
    void InitWindow();
    void InitMenu();
    void InitDimensionsBaseValue();
    void InitDimensionGradation(int index, const MeasurementDimension_p &dimension, QComboBox *control);
    void InitDimensionControls();
    void InitDimesionShifts();
    void InitTable();
    void SetDecimals();
    void InitPatternUnits();
    void InitComboBoxUnits();
    void InitMeasurementUnits();
    void InitGender(QComboBox *gender);

    void ShowNewMData(bool fresh);
    void ShowUnits();
    void ShowHeaderUnits(QTableWidget *table, int column, const QString &unit);

    void MeasurementsWereSaved(bool saved);
    void SetCurrentFile(const QString &fileName);
    bool SaveMeasurements(const QString &fileName, QString &error);

    bool MaybeSave();

    QTableWidgetItem *AddCell(const QString &text, int row, int column, int aligment, bool ok = true);

    void RefreshData(bool freshCall = false);
    void RefreshTable(bool freshCall = false);

    QString GetCustomName() const;
    QString ClearCustomName(const QString &name) const;

    bool EvalFormula(const QString &formula, bool fromUser, VContainer *data, QLabel *label, bool specialUnits);
    void ShowMDiagram(const QString &name);

    void Open(const QString &pathTo, const QString &filter);
    void UpdatePadlock(bool ro);
    void MeasurementGUI();
    void Controls();
    void MFields(bool enabled);
    void UpdateWindowTitle();

    void ReadSettings();
    void WriteSettings();

    QStringList FilterMeasurements(const QStringList &mNew, const QStringList &mFilter);

    void UpdatePatternUnit();

    bool LoadFromExistingFile(const QString &path);

    void CreateWindowMenu(QMenu *menu);

    bool IgnoreLocking(int error, const QString &path);

    template <class T>
    void HackWidget(T **widget);
    void HackDimensionBaseValue();
    void HackDimensionShifts();

    QString CheckMName(const QString &name, const QSet<QString> &importedNames) const;
    void ShowError(const QString &text);
    void RefreshDataAfterImport();

    void ImportIndividualMeasurements(const QxtCsvModel &csv);
    void ImportMultisizeMeasurements(const QxtCsvModel &csv);

    void SetCurrentPatternUnit();

    QString DimensionName(MeasurementDimension type);
    QString DimensionToolTip(MeasurementDimension type, bool circumference);

    void ShowDimensionControls();
    void SetDimensionBases();
    void SetCurrentDimensionValues();

    QVector<int> DimensionRestrictedValues(int index, const MeasurementDimension_p &dimension);

    QMap<int, QSharedPointer<VMeasurement> > OrderedMeasurments() const;
};

#endif // TMAINWINDOW_H
