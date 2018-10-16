namespace SettingsDialogCommon
{

/** display name with two lines that is displayed in the settings
 * If width is bigger than 0, the string will be elided so it does not exceed that width
 */
QString shortDisplayNameForSettings(Account* account, int width)
{
    QString user = account->davDisplayName();
    if (user.isEmpty()) {
        user = account->credentials()->user();
    }
    QString host = account->url().host();
    host = host.left(host.indexOf(".securium.ch"));
    if (width > 0) {
        QFont f;
        QFontMetrics fm(f);
        host = fm.elidedText(host, Qt::ElideMiddle, width);
        user = fm.elidedText(user, Qt::ElideRight, width);
    }
    return user + QLatin1String("\n") + host;
}

}