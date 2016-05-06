//   amtransportbutton.h
//
//   An audio transport button widget.
//
//   (C) Copyright 2002 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: amtransportbutton.h,v 1.1 2007/08/26 20:13:43 fredg Exp $
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Library General Public License 
//   version 2 as published by the Free Software Foundation.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public
//   License along with this program; if not, write to the Free Software
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
//

#ifndef AMTRANSPORTBUTTON_H
#define AMTRANSPORTBUTTON_H

#include <qwidget.h>
#include <qpushbutton.h>
#include <qpixmap.h>
#include <qcolor.h>

#define AMTRANSPORTBUTTON_DEFAULT_ON_COLOR green

/**
 * @short A set of audio transport buttons
 * @author Fred Gleason <fredg@wava.com
 * 
 * This class implements a standard QPushButton, with the addition of 
 * an (possibly animated) symbol.  Available symbols include one for each 
 * of the common audio functions:  Play, Stop, Fast Forward, Rewine, Pause, 
 * Record and Eject, along with two variations on play, PlayFrom and 
 * PlayBetween.  The symbols can be set to be dark, illuminated or
 * flash between those states.
 **/
class AMTransportButton : public QPushButton
{
  Q_OBJECT

  public:
  /**
   * Specify a transport function.
   **/
   enum TransType {Play=0,Stop=1,Record=2,FastForward=3,Rewind=4,Eject=5,
		   Pause=6,PlayFrom=7,PlayBetween=8,Loop=9,Up=10,Down=11,
		   PlayTo=12};

  /**
   * Specify the state of a transport button.
   **/ 
   enum TransState {On=0,Off=1,Flashing=2}; 

  /**
   * Instantiates the widget.
   * @param type The symbol to display on the button.
   * @param parent The parent widget.
   * @param name The name of the widget.
   **/
   AMTransportButton(AMTransportButton::TransType type,QWidget *parent,
		    const char *name);

  /**
   * Returns the symbol displayed on the button.
   **/
   AMTransportButton::TransType getType() const;

  /**
   * Set the symbol to display on the button.
   * @param type The symbol to display on the button.
   **/
   void setType(AMTransportButton::TransType type);

  /** 
   * Get the "on" color of the button
   **/
   QColor onColor() const;

  /**
   * Set the "on" color of the button
   **/
   void setOnColor(QColor color);

  /** 
   * Get the "accent" color of the button
   **/
   QColor accentColor() const;

  /**
   * Set the "accent" color of the button
   **/
   void setAccentColor(QColor color);

  /**
   * Set the state of the button.  Available states are On, Off or Flashing.
   * @param state The state to place the button into.
   **/
   void setState(AMTransportButton::TransState state);

  public slots:

  /**
   * Place the button into the 'on' state
   **/
   void on();

  /**
   * Place the button into the 'off' state
   **/
   void off();

  /**
   * Place the button into the 'flashing' state
   **/
   void flash();

  protected:
   virtual void resizeEvent(QResizeEvent *);
   virtual void enabledChange(bool oldEnabled);

  private slots:
   void flashClock();

  private:
   void updateCaps();
   void drawMask(QPixmap *cap);
   void drawOnCap();
   void drawOffCap();
   void flashOn();
   void flashOff();
   bool flash_state;
   AMTransportButton::TransType button_type;
   AMTransportButton::TransState button_state;
   QColor on_color;
   QColor accent_color;
   QPixmap *on_cap;
   QPixmap *off_cap;
   QTimer *flash_timer;
};



#endif  // AMTRANSPORTBUTTON_H
