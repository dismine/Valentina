#ifndef VPUZZLECOMMANDLINE_H
#define VPUZZLECOMMANDLINE_H

#include <memory>
#include <vector>
#include <QTextStream>
#include <QCoreApplication>
#include <QCommandLineParser>

class VPuzzleCommandLine: public QObject
{
    Q_OBJECT
public:   
    virtual ~VPuzzleCommandLine() = default;

    /**
     * @brief if user enabled export from cmd
     */
    bool IsExportEnabled() const;
    /**
     * @brief the base name of layout file or empty string if not
     */
    QString OptionBaseName() const;
    /**
     * @brief if user enabled test mode from cmd
     */
    bool IsTestModeEnabled() const;
    /**
     * @brief if gui enabled or not
     */
    bool IsGuiEnabled() const;
    /**
     * @brief the file name which should be loaded
     */
    QString OptionFileName() const;
protected:
    VPuzzleCommandLine();
    /**
     * @brief create the single instance of the class inside puzzleapplication
     */
    static std::shared_ptr<VPuzzleCommandLine> Instance(const QCoreApplication &app);
private:
    Q_DISABLE_COPY(VPuzzleCommandLine)
    static std::shared_ptr<VPuzzleCommandLine> instance;
    QCommandLineParser parser;
    bool isGuiEnabled;

    /**
     * @brief add options to the QCommandLineParser that there are in the cmd can be
     */
    void InitCommandLineOptions();
    bool IsOptionSet(const QString &option)const;
    QString OptionValue(const QString &option) const;
    QStringList OptionValues(const QString &option) const;
};

#endif // VPUZZLECOMMANDLINE_H
