// Here we avoid loading the header multiple times
#ifndef Tutorial1_HEADER
#define Tutorial1_HEADER
// We need the Plasma Applet headers
#include <KIcon>
 
#include <Plasma/Applet>
#include <Plasma/Svg>
 #include <KProcess>

class QProgressBar;
class KProgressDialog;
class KIntNumInput;
class QCheckBox;
class QPushButton;
class QSizeF;
class QDialog;
class QProgressBar;
class QLabel;

// Define our plasma Applet
class Plasmaeyesaver : public Plasma::Applet
{
    Q_OBJECT
    public:
        // Basic Create/Destroy
        Plasmaeyesaver(QObject *parent, const QVariantList &args);
        ~Plasmaeyesaver();
 
        // The paintInterface procedure paints the applet to screen
        void paintInterface(QPainter *painter,
                const QStyleOptionGraphicsItem *option,
                const QRect& contentsRect);
	void init();
        virtual QList<QAction *> contextualActions();
    protected:
        void createConfigurationInterface( KConfigDialog *p_parent );
    private slots:
	void aggiorna();
        void saveConfig();
        void loadConfig();
        void riavvia();
        void apridocum();
        void disattiva();
	void inizia_pausa();
	void rifiuta_pausa();
	void fine_pausa();
	void pausa_pausa();
    private:
        QList<QAction *> m_actions;
        Plasma::Svg m_svg;
	QTimer *m_timer;
	QString filename;
	KProcess m_proc;
	Plasma::Svg *m_btn;
	QRectF m_mascherino;
	float secondo;
        int secscritti;
        KIntNumInput *m_durata_pausa;
        int duratapausa;
        KIntNumInput *m_quando_pausa;
        int quandopausa;
        QCheckBox *m_musichetta;
        bool disattivato;
        bool musichetta;
        QPushButton *m_riavvia;
        bool in_pausa;
	QPointer<QDialog> progresso;
	QProgressBar *barra;
	QLabel *testo_prog;
	QPushButton *pausa_prog;
        //KProgressDialog progresso;
};
 
// This is the command that links your applet to the .desktop file
K_EXPORT_PLASMA_APPLET(eyesaver, Plasmaeyesaver)
#endif 

