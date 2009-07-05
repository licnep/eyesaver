#include "eyesaver.h"
#include <QAction>
#include <QPainter>
#include <QFontMetrics>
#include <QSizeF>
#include <QTimer> 
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QDialog>
#include <KGlobalSettings>
#include <plasma/svg.h>
#include <plasma/theme.h>
#include <Phonon/MediaObject> 
#include <KConfigDialog>
#include <KIntNumInput>
#include <KPassivePopup>
#include <KProgressDialog>
#include <QProgressBar>
#include <QCheckBox>
#include <QGroupBox>
#include <QVBoxLayout>
#include <KRun>
#include <KUrl>
#include <KStandardDirs>
#include <KWindowSystem>
#include <NETRootInfo>

Plasmaeyesaver::Plasmaeyesaver(QObject *parent, const QVariantList &args)
    : Plasma::Applet(parent, args),
    m_svg(this)
 {
    m_svg.setImagePath("widgets/background");
    m_btn = new Plasma::Svg(this);
    m_btn->setImagePath("widgets/occhio2");
    // this will get us the standard applet background, for free!
    setBackgroundHints(DefaultBackground);
    setHasConfigurationInterface(true);
    resize(180, 95);
    }
 
 
Plasmaeyesaver::~Plasmaeyesaver()
{
    ///delete m_configDialog;
    if (progresso)  delete progresso;
}
 
void Plasmaeyesaver::init()
{
 
    m_timer = new QTimer( this );
    m_timer->setSingleShot( true );
    connect( m_timer, SIGNAL( timeout() ), SLOT( aggiorna() ) );
    QAction *azioneDisattiva = new QAction( SmallIcon("system-run"), i18n("Turn Off"), this );
    connect( azioneDisattiva, SIGNAL( triggered(bool) ), SLOT( disattiva() ) );
    m_actions.append( azioneDisattiva );
    
    disattivato=0;
    m_timer->start( 4000 );
    loadConfig();
    secondo=0;
    in_pausa = false;
} 


void Plasmaeyesaver::loadConfig ()
{
   KConfigGroup cfg = config();
   duratapausa = cfg.readEntry("duratapausa", 900);
   quandopausa = cfg.readEntry("quandopausa", 2700);
   disattivato = cfg.readEntry("disattivato", 0);
   musichetta =  cfg.readEntry("musichetta", 1);
}

void Plasmaeyesaver::saveConfig ()
{
   KConfigGroup cfg = config();
   duratapausa = m_durata_pausa->value()*60;
   quandopausa = m_quando_pausa->value()*60;
   musichetta = m_musichetta->isChecked();
   cfg.writeEntry("quandopausa", quandopausa);
   cfg.writeEntry("duratapausa", duratapausa);
   cfg.writeEntry("disattivato", disattivato);
   cfg.writeEntry("musichetta",musichetta);
}

void Plasmaeyesaver::apridocum () {
    KUrl *doc = new KUrl("http://spitalia.altervista.org/blog/?page_id=18");
    new KRun::KRun(*doc,0);
}

void Plasmaeyesaver::createConfigurationInterface( KConfigDialog *p_parent )
{
    ///pannello configurazione:::
    p_parent->setButtons( KDialog::Ok | KDialog::Cancel | KDialog::Apply );
    connect( p_parent, SIGNAL( applyClicked() ), SLOT( saveConfig() ) );
    connect( p_parent, SIGNAL( okClicked() ), SLOT( saveConfig() ) );


    QWidget *page = new QWidget();
    QLabel *quando_label = new QLabel( i18n("Pause every: (min) "), page );
    QLabel *durata_label = new QLabel( i18n("Pause duration: (min) "), page );
    
    m_durata_pausa = new KIntNumInput( page );
    m_durata_pausa->setMinimum( 1 );
    m_durata_pausa->setValue(duratapausa/60);
    m_quando_pausa = new KIntNumInput( page );
    m_quando_pausa->setMinimum( 1 );   
    m_quando_pausa->setValue(quandopausa/60);
    m_musichetta = new QCheckBox("Play sound", page);
    m_musichetta->setChecked(musichetta);
    m_riavvia = new QPushButton("Restart", page);
    connect(m_riavvia, SIGNAL( clicked() ), SLOT(riavvia()));
    connect(m_riavvia, SIGNAL( clicked() ), SLOT(saveConfig()));
    
    QGroupBox *groupBox = new QGroupBox( page );
    QLabel *info_default = new QLabel("For default settings and what to <br> do during the pause click <a href=\"http://spitalia.altervista.org/blog/?page_id=18\">here</a>");
    //info_default->setOpenExternalLinks(true);
    connect(info_default, SIGNAL( linkActivated( const QString &) ), SLOT( apridocum() ));
    QVBoxLayout *stupidvbox = new QVBoxLayout;
    stupidvbox->addWidget(info_default);
    groupBox->setLayout(stupidvbox);   

    QVBoxLayout *fette = new QVBoxLayout( page );
    QGridLayout *layout = new QGridLayout();
    layout->setColumnStretch( 1, 1 );
    layout->addWidget( quando_label, 0, 0 );
    layout->addWidget( m_quando_pausa, 0, 1 );
    layout->addWidget( durata_label, 1, 0 );
    layout->addWidget( m_durata_pausa, 1, 1 );
    layout->addWidget( m_musichetta, 2, 0 );
    layout->addWidget( m_riavvia, 3, 1 );
    fette->addLayout( layout );
    fette->addWidget( groupBox );
    p_parent->addPage( page, i18nc( "name of the applet", "%1 Settings", name() ), icon() );
}

void Plasmaeyesaver::riavvia ()
{
  secondo=0;
  in_pausa = false;
  aggiorna();
}

void Plasmaeyesaver::disattiva ()
{
  if(!disattivato) {
    disattivato=1;
    if(in_pausa) fine_pausa();
    //m_disattiva->setText("Turn on");
  }
  else  {
    disattivato=0;
    //m_disattiva->setText("Turn off");
  }
  aggiorna();
}

void Plasmaeyesaver::aggiorna()
{
  if (!disattivato) {
  if (!in_pausa){
    secondo += 4;
    if (secondo>=quandopausa)
    {
      if(musichetta) {
	 Phonon::MediaObject *music = Phonon::createPlayer(Phonon::NotificationCategory, 
							   Phonon::MediaSource( KStandardDirs::locate( "sound", "KDE-Sys-Log-In-Short.ogg" ) ));
	 music->play();
      }
      QMessageBox *message;
      message = new QMessageBox(QMessageBox::Warning,"Eyesaver","It's time to take a break,\npress Ok when you're ready");
      QPushButton *bottone_rifiuta;
      QPushButton *bottone_spegni;
      QPushButton *bottone_ok;
      bottone_ok = message->addButton("Ok",QMessageBox::AcceptRole);
      bottone_rifiuta = message->addButton("Skip this pause",QMessageBox::RejectRole);
      bottone_spegni = message->addButton("Turn eyesaver off",QMessageBox::RejectRole);
      
      connect(bottone_spegni, SIGNAL( clicked() ),SLOT( disattiva() ));
      connect(bottone_rifiuta, SIGNAL( clicked() ), SLOT(rifiuta_pausa()) );
      connect(message, SIGNAL( rejected()), SLOT( rifiuta_pausa()) );
      connect(bottone_ok, SIGNAL(clicked()), SLOT(inizia_pausa()));
      message->open();//(this, SLOT(inizia_pausa()) );
      //I try to put "message" on top of other windows:
      KWindowSystem::setState(message->winId(), NET::KeepAbove | NET::Sticky);
      
    }
    else m_timer->start( 4000 );
  }
  else {
    secondo += 1;
    secscritti = duratapausa-secondo-60*int((duratapausa-secondo)/60);
    testo_prog->setText(i18n("Time remaining : <em>%1</em> minute/s", int((duratapausa-secondo)/60)+1));
    barra->setValue(secondo);
    if (secondo==duratapausa)
    {  
      if(musichetta) {
	 Phonon::MediaObject *music = Phonon::createPlayer(Phonon::NotificationCategory, 
							   Phonon::MediaSource( KStandardDirs::locate( "sound", "KDE-Im-Phone-Ring.ogg" ) ));
	 music->play();
      }
      fine_pausa();
    }
    m_timer->start( 1000 );
  }
  }
  else //se e' disattivato
  {
    secondo=0;
  }
    update();
}
 
void Plasmaeyesaver::paintInterface(QPainter *p,
        const QStyleOptionGraphicsItem *option, const QRect &contentsRect)
{
    p->setRenderHint(QPainter::SmoothPixmapTransform);
    p->setRenderHint(QPainter::Antialiasing);
    
    // Now we draw the applet, starting with our svg
    
    m_btn->resize((int)contentsRect.width(), (int)contentsRect.height());
    m_btn->paint(p, (int)contentsRect.left(), (int)contentsRect.top(), "ferro");
    if (!disattivato) {
    //metto il mascherino a seconda della pausa
    if (!in_pausa) {
    p->setClipRect(contentsRect.left(),contentsRect.top(),(int)(contentsRect.width()/9.5+(contentsRect.width()-contentsRect.width()/4.6)*(secondo/quandopausa)),contentsRect.height());
    }
    else {
      p->setClipRect(contentsRect.left(),contentsRect.top(),(int)(contentsRect.width()*8.5/9.5-(contentsRect.width()-contentsRect.width()/4.6)*(secondo/duratapausa)),contentsRect.height());
    }
    }
    m_btn->paint(p, (int)(contentsRect.left()+contentsRect.width()/9.5), (int)contentsRect.top()+contentsRect.width()/14, "sangue");
    p->setClipping(0);
    p->setClipRect(contentsRect.left(),contentsRect.top(),contentsRect.width(),contentsRect.height());
    m_btn->paint(p, (int)(contentsRect.left()+contentsRect.width()/9.5), (int)contentsRect.top()+contentsRect.width()/14, "vetro");
}
 
QList<QAction *> Plasmaeyesaver::contextualActions()
{
  if (!disattivato) {
    QAction *azioneDisattiva = new QAction( SmallIcon("media-playback-pause"), i18n("Turn Off"), this );
    connect( azioneDisattiva, SIGNAL( triggered(bool) ), SLOT( disattiva() ) );
    m_actions.clear();
    m_actions.append( azioneDisattiva );
  }
  else {
    QAction *azioneDisattiva = new QAction( SmallIcon("media-playback-start"), i18n("Turn On"), this );
    connect( azioneDisattiva, SIGNAL( triggered(bool) ), SLOT( disattiva() ) );
    m_actions.clear();
    m_actions.append( azioneDisattiva );
  }
  return m_actions;
}


void Plasmaeyesaver::inizia_pausa () {
   in_pausa=true;
   secondo=0;
   m_timer->start( 1000 );
   update();
   
   progresso = new QDialog();
   progresso->setWindowTitle("Eyesaver - PAUSE");
   barra = new QProgressBar(progresso);
   barra->setMaximum(duratapausa);
   barra->setMinimumWidth(400);
   testo_prog = new QLabel(progresso);
   testo_prog->setText(i18n("Time remaining : <em>%1</em> minute/s", int((duratapausa-secondo)/60)));
   testo_prog->setAlignment(Qt::AlignHCenter);
   pausa_prog = new QPushButton(SmallIcon("media-playback-pause"),"",progresso);
   pausa_prog->setMaximumWidth(30);
   QPushButton *salta_prog = new QPushButton(SmallIcon("media-playback-stop"),"",progresso);
   salta_prog->setMaximumWidth(30);
   
   QHBoxLayout *barra_e_testo = new QHBoxLayout;
   barra_e_testo->addWidget(barra);
   barra_e_testo->addWidget(pausa_prog);
   barra_e_testo->addWidget(salta_prog);
   
   QVBoxLayout *vlayout = new QVBoxLayout;
   vlayout->addWidget(testo_prog);
   vlayout->addLayout(barra_e_testo);
   
   progresso->setLayout(vlayout);
   
   connect(progresso, SIGNAL(rejected()), SLOT(fine_pausa()));
   connect(pausa_prog, SIGNAL(clicked()), SLOT(pausa_pausa()));
   connect(salta_prog, SIGNAL(clicked()), SLOT(fine_pausa()));
   
   progresso->show();
   KWindowSystem::setState(progresso->winId(), NET::KeepAbove | NET::Sticky);
}

void Plasmaeyesaver::rifiuta_pausa () {
   secondo = 0;
   in_pausa=false;
   m_timer->start( 4000 );
   update();
}

void Plasmaeyesaver::fine_pausa () {
   in_pausa=false;
   if(progresso) progresso->close();
   secondo=0;
   m_timer->start(1000);
}

void Plasmaeyesaver::pausa_pausa () {
   if (m_timer->isActive()) {
      m_timer->stop();
      barra->setMaximum(0);
      pausa_prog->setIcon(SmallIcon("media-playback-start"));
   } else {//already in pause-pausa 
      m_timer->start(1000);
      barra->setMaximum(duratapausa);
      barra->setValue(secondo);
      pausa_prog->setIcon(SmallIcon("media-playback-pause"));
   }
}

#include "eyesaver.moc"
