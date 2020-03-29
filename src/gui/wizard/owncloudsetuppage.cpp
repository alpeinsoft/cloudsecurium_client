/*
 * Copyright (C) by Klaas Freitag <freitag@owncloud.com>
 * Copyright (C) by Krzesimir Nowak <krzesimir@endocode.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 */

#include <QDir>
#include <QFileDialog>
#include <QUrl>
#include <QTimer>
#include <QPushButton>
#include <QMessageBox>
#include <QSsl>
#include <QSslCertificate>
#include <QNetworkAccessManager>
#include <QPropertyAnimation>
#include <QGraphicsPixmapItem>

#include "QProgressIndicator.h"

#include "wizard/owncloudwizardcommon.h"
#include "wizard/owncloudsetuppage.h"
#include "wizard/owncloudconnectionmethoddialog.h"
#include "wizard/slideshow.h"
#include "theme.h"
#include "account.h"
#include "config.h"

namespace OCC {

OwncloudSetupPage::OwncloudSetupPage(QWidget *parent)
    : QWizardPage()
    , _ui()
    , _oCUrl()
    , _ocUser()
    , _authTypeKnown(false)
    , _checking(false)
    , _authType(DetermineAuthTypeJob::Basic)
    , _progressIndi(new QProgressIndicator(this))
{
    _ui.setupUi(this);
    _ocWizard = qobject_cast<OwncloudWizard *>(parent);

    Theme *theme = Theme::instance();
    setTitle(WizardCommon::titleTemplate().arg(tr("Connect to %1").arg(theme->appNameGUI())));
    setSubTitle(WizardCommon::subTitleTemplate().arg(tr("Setup %1 server").arg(theme->appNameGUI())));

    if (theme->overrideServerUrl().isEmpty()) {
        _ui.leUrl->setPostfix(theme->wizardUrlPostfix());
        _ui.leUrl->setPlaceholderText(theme->wizardUrlHint());
    } else {
        _ui.leUrl->setEnabled(false);
    }


    registerField(QLatin1String("OCUrl*"), _ui.leUrl);

    _ui.resultLayout->addWidget(_progressIndi);
    stopSpinner();

    setupCustomization();

    slotUrlChanged(QLatin1String("")); // don't jitter UI
    connect(_ui.leUrl, &QLineEdit::textChanged, this, &OwncloudSetupPage::slotUrlChanged);

    addCertDial = new AddCertificateDialog(this);

#ifdef WITH_PROVIDERS
    connect(_ui.loginButton, &QPushButton::clicked, this, &OwncloudSetupPage::slotLogin);
    connect(_ui.createAccountButton, &QPushButton::clicked, this, &OwncloudSetupPage::slotGotoProviderList);

    _ui.login->hide();
    _ui.slideShow->addSlide(Theme::hidpiFileName(":/client/theme/colored/wizard-nextcloud.png"), tr("Keep your data secure and under your control"));
    _ui.slideShow->addSlide(Theme::hidpiFileName(":/client/theme/colored/wizard-files.png"), tr("Secure collaboration & file exchange"));
    _ui.slideShow->addSlide(Theme::hidpiFileName(":/client/theme/colored/wizard-groupware.png"), tr("Easy-to-use web mail, calendaring & contacts"));
    _ui.slideShow->addSlide(Theme::hidpiFileName(":/client/theme/colored/wizard-talk.png"), tr("Screensharing, online meetings & web conferences"));
    connect(_ui.slideShow, &SlideShow::clicked, _ui.slideShow, &SlideShow::stopShow);
    connect(_ui.nextButton, &QPushButton::clicked, _ui.slideShow, &SlideShow::nextSlide);
    connect(_ui.prevButton, &QPushButton::clicked, _ui.slideShow, &SlideShow::prevSlide);

    _ui.slideShow->startShow();
#else
    _ui.createAccountButton->hide();
    _ui.loginButton->hide();
    _ui.installLink->hide();
    _ui.slideShow->hide();
#endif

    customizeStyle();
}

void OwncloudSetupPage::setServerUrl(const QString &newUrl)
{
    _ocWizard->setRegistration(false);
    _oCUrl = newUrl;
    if (_oCUrl.isEmpty()) {
        _ui.leUrl->clear();
        return;
    }

    _ui.leUrl->setText(_oCUrl);
}

void OwncloudSetupPage::setupCustomization()
{
    // set defaults for the customize labels.
    _ui.topLabel->hide();
    _ui.bottomLabel->hide();

    Theme *theme = Theme::instance();
    QVariant variant = theme->customMedia(Theme::oCSetupTop);
    if (!variant.isNull()) {
        WizardCommon::setupCustomMedia(variant, _ui.topLabel);
    }

    variant = theme->customMedia(Theme::oCSetupBottom);
    WizardCommon::setupCustomMedia(variant, _ui.bottomLabel);
}

#ifdef WITH_PROVIDERS
void OwncloudSetupPage::slotLogin()
{
    _ocWizard->setRegistration(false);
    _ui.login->setMaximumHeight(0);
    QPropertyAnimation *animation = new QPropertyAnimation(_ui.login, "maximumHeight");
    animation->setDuration(0);
    animation->setStartValue(500);
    animation->setEndValue(500);
    _ui.login->show();
    _ui.loginButton->hide();
    wizard()->resize(wizard()->sizeHint());
    animation->start();
}
void OwncloudSetupPage::slotGotoProviderList()
{
    _ocWizard->setRegistration(true);
    _ocWizard->setAuthType(DetermineAuthTypeJob::AuthType::WebViewFlow);
    _authTypeKnown = true;
    _checking = false;
    emit completeChanged();
}
#endif

// slot hit from textChanged of the url entry field.
void OwncloudSetupPage::slotUrlChanged(const QString &url)
{
    _authTypeKnown = false;

    QString newUrl = url;
    if (url.endsWith("index.php")) {
        newUrl.chop(9);
    }
    if (_ocWizard && _ocWizard->account()) {
        QString webDavPath = _ocWizard->account()->davPath();
        if (url.endsWith(webDavPath)) {
            newUrl.chop(webDavPath.length());
        }
        if (webDavPath.endsWith(QLatin1Char('/'))) {
            webDavPath.chop(1); // cut off the slash
            if (url.endsWith(webDavPath)) {
                newUrl.chop(webDavPath.length());
            }
        }
    }
    if (newUrl != url) {
        _ui.leUrl->setText(newUrl);
    }

    if (!url.startsWith(QLatin1String("https://"))) {
        _ui.urlLabel->setPixmap(QPixmap(Theme::hidpiFileName(":/client/resources/lock-http.png")));
        _ui.urlLabel->setToolTip(tr("This URL is NOT secure as it is not encrypted.\n"
                                    "It is not advisable to use it."));
    } else {
        _ui.urlLabel->setPixmap(QPixmap(Theme::hidpiFileName(":/client/resources/lock-https.png")));
        _ui.urlLabel->setToolTip(tr("This URL is secure. You can use it."));
    }
}

void OwncloudSetupPage::slotUrlEditFinished()
{
    QString url = _ui.leUrl->fullText();
    if (QUrl(url).isRelative() && !url.isEmpty()) {
        // no scheme defined, set one
        url.prepend("https://");
    }
    _ui.leUrl->setFullText(url);
}

bool OwncloudSetupPage::isComplete() const
{
    return !_ui.leUrl->text().isEmpty() && !_checking;
}

void OwncloudSetupPage::initializePage()
{
    WizardCommon::initErrorLabel(_ui.errorLabel);

    _authTypeKnown = false;
    _checking = false;

    QAbstractButton *nextButton = wizard()->button(QWizard::NextButton);
    QPushButton *pushButton = qobject_cast<QPushButton *>(nextButton);
    if (pushButton)
        pushButton->setDefault(true);

    // If url is overriden by theme, it's already set and
    // we just check the server type and switch to second page
    // immediately.
    if (Theme::instance()->overrideServerUrl().isEmpty()) {
        _ui.leUrl->setFocus();
    } else {
        setCommitPage(true);
        // Hack: setCommitPage() changes caption, but after an error this page could still be visible
        setButtonText(QWizard::CommitButton, tr("&Next >"));
        validatePage();
        setVisible(false);
    }
    wizard()->resize(wizard()->sizeHint());
}

bool OwncloudSetupPage::urlHasChanged()
{
    bool change = false;
    const QChar slash('/');

    QUrl currentUrl(url());
    QUrl initialUrl(_oCUrl);

    QString currentPath = currentUrl.path();
    QString initialPath = initialUrl.path();

    // add a trailing slash.
    if (!currentPath.endsWith(slash))
        currentPath += slash;
    if (!initialPath.endsWith(slash))
        initialPath += slash;

    if (currentUrl.host() != initialUrl.host() || currentUrl.port() != initialUrl.port() || currentPath != initialPath) {
        change = true;
    }

    return change;
}

int OwncloudSetupPage::nextId() const
{
    switch (_authType) {
    case DetermineAuthTypeJob::Basic:
        return WizardCommon::Page_HttpCreds;
    case DetermineAuthTypeJob::OAuth:
        return WizardCommon::Page_OAuthCreds;
    case DetermineAuthTypeJob::LoginFlowV2:
        return WizardCommon::Page_Flow2AuthCreds;
    case DetermineAuthTypeJob::Shibboleth:
        return WizardCommon::Page_ShibbolethCreds;
    case DetermineAuthTypeJob::WebViewFlow:
        return WizardCommon::Page_WebView;
    }
    return WizardCommon::Page_HttpCreds;
}

QString OwncloudSetupPage::url() const
{
    QString url = _ui.leUrl->fullText().simplified();
    return QString("https://%1.securium.ch/cloudsecurium").arg(url);
}

bool OwncloudSetupPage::validatePage()
{
    if (!_authTypeKnown) {
        QString u = url();
        QUrl qurl(u);
        if (!qurl.isValid() || qurl.host().isEmpty()) {
            setErrorString(tr("Invalid URL"), false);
            return false;
        }

        setErrorString(QString(), false);
        _checking = true;
        startSpinner();
        emit completeChanged();

        emit determineAuthType(u);
        return false;
    } else {
        // connecting is running
        stopSpinner();
        _checking = false;
        emit completeChanged();
        return true;
    }
}

void OwncloudSetupPage::setAuthType(DetermineAuthTypeJob::AuthType type)
{
    _authTypeKnown = true;
    _authType = type;
    stopSpinner();
}

void OwncloudSetupPage::setErrorString(const QString &err, bool retryHTTPonly)
{
    if (err.isEmpty()) {
        _ui.errorLabel->setVisible(false);
    } else {
        if (retryHTTPonly) {
            QUrl url(_ui.leUrl->fullText());
            if (url.scheme() == "https") {
                // Ask the user how to proceed when connecting to a https:// URL fails.
                // It is possible that the server is secured with client-side TLS certificates,
                // but that it has no way of informing the owncloud client that this is the case.

                OwncloudConnectionMethodDialog dialog;
                dialog.setUrl(url);
                // FIXME: Synchronous dialogs are not so nice because of event loop recursion
                int retVal = dialog.exec();

                switch (retVal) {
                case OwncloudConnectionMethodDialog::No_TLS: {
                    url.setScheme("http");
                    _ui.leUrl->setFullText(url.toString());
                    // skip ahead to next page, since the user would expect us to retry automatically
                    wizard()->next();
                } break;
                case OwncloudConnectionMethodDialog::Client_Side_TLS:
                    addCertDial->show();
                    connect(addCertDial, &QDialog::accepted, this, &OwncloudSetupPage::slotCertificateAccepted);
                    break;
                case OwncloudConnectionMethodDialog::Closed:
                case OwncloudConnectionMethodDialog::Back:
                default:
                    // No-op.
                    break;
                }
            }
        }

        _ui.errorLabel->setVisible(true);
        _ui.errorLabel->setText("Failed to connect to CloudSecurium. Please enter valid product ID.");
    }
    _checking = false;
    emit completeChanged();
    stopSpinner();
    wizard()->resize(wizard()->sizeHint());
}

void OwncloudSetupPage::startSpinner()
{
    _ui.resultLayout->setEnabled(true);
    _ui.urlLabel->setVisible(false);
    _progressIndi->setVisible(true);
    _progressIndi->startAnimation();
}

void OwncloudSetupPage::stopSpinner()
{
    _ui.resultLayout->setEnabled(false);
    _ui.urlLabel->setVisible(true);
    _progressIndi->setVisible(false);
    _progressIndi->stopAnimation();
}

QString subjectInfoHelper(const QSslCertificate &cert, const QByteArray &qa)
{
    return cert.subjectInfo(qa).join(QLatin1Char('/'));
}

//called during the validation of the client certificate.
void OwncloudSetupPage::slotCertificateAccepted()
{
    QFile certFile(addCertDial->getCertificatePath());
    certFile.open(QFile::ReadOnly);
    if (QSslCertificate::importPkcs12(
            &certFile,
            &_ocWizard->_clientSslKey,
            &_ocWizard->_clientSslCertificate,
            &_ocWizard->_clientSslCaCertificates,
            addCertDial->getCertificatePasswd().toLocal8Bit())) {
        AccountPtr acc = _ocWizard->account();

        // to re-create the session ticket because we added a key/cert
        acc->setSslConfiguration(QSslConfiguration());
        QSslConfiguration sslConfiguration = acc->getOrCreateSslConfig();

        // We're stuffing the certificate into the configuration form here. Later the
        // cert will come via the HttpCredentials
        sslConfiguration.setLocalCertificate(_ocWizard->_clientSslCertificate);
        sslConfiguration.setPrivateKey(_ocWizard->_clientSslKey);

        // Be sure to merge the CAs
        auto ca = sslConfiguration.systemCaCertificates();
        ca.append(_ocWizard->_clientSslCaCertificates);
        sslConfiguration.setCaCertificates(ca);

        acc->setSslConfiguration(sslConfiguration);

        // Make sure TCP connections get re-established
        acc->networkAccessManager()->clearAccessCache();

        addCertDial->reinit(); // FIXME: Why not just have this only created on use?
        validatePage();
    } else {
        addCertDial->showErrorMessage(tr("Could not load certificate. Maybe wrong password?"));
        addCertDial->show();
    }
}

OwncloudSetupPage::~OwncloudSetupPage()
{
}

void OwncloudSetupPage::slotStyleChanged()
{
    customizeStyle();
}

void OwncloudSetupPage::customizeStyle()
{
#ifdef WITH_PROVIDERS
    Theme *theme = Theme::instance();

    bool widgetHasDarkBg = Theme::isDarkColor(QGuiApplication::palette().base().color());
    _ui.nextButton->setIcon(theme->uiThemeIcon(QString("control-next.svg"), widgetHasDarkBg));
    _ui.prevButton->setIcon(theme->uiThemeIcon(QString("control-prev.svg"), widgetHasDarkBg));

    // QPushButtons are a mess when it comes to consistent background coloring without stylesheets,
    // so we do it here even though this is an exceptional styling method here
    _ui.createAccountButton->setStyleSheet("QPushButton {background-color: #0082C9; color: white}");

    QPalette pal = _ui.slideShow->palette();
    pal.setColor(QPalette::WindowText, theme->wizardHeaderBackgroundColor());
    _ui.slideShow->setPalette(pal);
#endif

    if(_progressIndi)
        _progressIndi->setColor(QGuiApplication::palette().color(QPalette::Text));
}

} // namespace OCC
