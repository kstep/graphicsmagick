    //  ------------------------------------------------------------------------------------------------
    //  MODULE    : Graphique
    //  LANGAGE   : C++
    //  CREATEUR  : Adolfo VIDE
    //  DATE    : October 15, 1992
    //  DESCRIPTION : Geometical classes with integer coordinates
    //  COMMENTAIRE :
  //  SCCSID      : @(#)graphic.h 1.1 11:50:02 18 Dec 1996
  //  ----------------------------------------------------------------------------
  //  Copyright (c) 1999 Digital Imaging Group, Inc.
  //  For conditions of distribution and use, see copyright notice
  //  in Flashpix.h
  //  ----------------------------------------------------------------------------
    //  ------------------------------------------------------------------------------------------------
      #ifndef Graphique_h
      #define Graphique_h
      #ifndef ApplCommun_h
        #include  "common_a.h"  // GDN - many things compile w/o this
      #endif
    //  ------------------------------------------------------------------------------------------------
    
    //  Includes
    //  --------
        
    // to do not include visuel
      class obj_Visuel;
      typedef obj_Visuel* ptr_Visuel;
      typedef obj_Visuel& ref_Visuel;
      
    //  Constants
    //  ---------
    
    //  Classes declaration
    //  -------------------
      
      struct Position;
      typedef Position* ptr_Position;
      typedef Position& ref_Position;
      
      struct PRectangle;
      typedef PRectangle* ptr_PRectangle;
      typedef PRectangle& ref_PRectangle;
      
      // Backward compatibility
      #define Rectangle  PRectangle
      typedef Rectangle* ptr_Rectangle;
      typedef Rectangle& ref_Rectangle;
      
  #ifdef macintosh
    
      struct Focus;
      typedef Focus* ptr_NouveauPort;
      typedef Focus& ref_NouveauPort;
      
  #endif
        
    //  Classes definition
    //  ------------------
    
  #ifdef macintosh
  
      struct Focus {
                    Focus(GrafPtr newGrafPtr);
                    ~Focus();
      
      private:
            GrafPtr     oldGrafPtr;
      };
      
    #endif
    
    // Structure alignment directive for PowerMac
    #ifdef powerc
    # pragma options align=mac68k
    #endif
    #ifdef __powerc
    # pragma options align=mac68k
    #endif
    
    
      struct Position {
                    int16 v, h;         // GDN - replicate Mac struct Point, do not change!
      
                    Position();
                    Position(short x, short y);
      
                    // Is Position inside a rectangle?
            Boolean     Interne(short x0, short y0, short x1, short y1) const;
                    
        friend  int       operator ==(const Position& p1, const Position& p2);
        friend  int       operator !=(const Position& p1, const Position& p2);
        friend  Position    operator +(Position p1, const Position& p2);
        friend  Position    operator -(Position p1, const Position& p2);
            Position    operator +=(const Position& p);
            Position    operator -=(const Position& p);
      };


#ifdef macintosh
      struct PRectangle : public Rect {
                    PRectangle(const Rect& rect);
#else    
    
      struct PRectangle {                   // GDN - replicate Mac struct Rect, do not change!
                    int16 top, left, bottom, right; // Note: left/right are last column/row + 1
#endif                    
    
                    PRectangle();
                    ~PRectangle();
                    PRectangle(const Position& p1, const Position& p2);
                    PRectangle(short x0, short y0, short x1, short y1);
                    
            short     Largeur() const;          // Width
            short     Hauteur() const;          // Height
            
            Boolean     Contient(const Position& p) const;
            
            Boolean     IsEmpty();
          
        friend  int       operator ==(const PRectangle &r1, const PRectangle &r2);
        friend  int       operator !=(const PRectangle &r1, const PRectangle &r2);
                    // Shift origin
            PRectangle    operator +=(const Position& p);
            PRectangle    operator -=(const Position& p);
                    // Union of rectangles
            void      Union(const PRectangle& rect);
            PRectangle    operator |=(const PRectangle& r); 
        friend  PRectangle    operator |(const PRectangle &r1, const PRectangle &r2);
                    // Intersection of rectangles
            void      Intersection(const PRectangle& rect);
            PRectangle    operator &=(const PRectangle& r); 
        friend  PRectangle    operator &(const PRectangle &r1, const PRectangle &r2);
      };
      
    
    // Structure alignment directive for PowerMac
    #ifdef powerc
    # pragma options align=reset
    #endif
    #ifdef __powerc
    # pragma options align=reset
    #endif
    
    
    //  Fonctions 'inline'
    //  ------------------
    
  #ifdef macintosh
    
      // Set the current graphic port (offscreen or window)
      #define NouveauPort(a)        Focus newGraphicPort(a)
      
      inline Focus::Focus(GrafPtr newGrafPtr)
      
      {
        GetPort(&oldGrafPtr);
        SetPort(newGrafPtr);
      }
      
      inline Focus::~Focus()
      
      {
        SetPort(oldGrafPtr);
      }
      
    #endif
          
      inline PRectangle::~PRectangle() { }
        
    //  ------------------------------------------------------------------------------------------------
      #endif // Graphique_h
    //  ------------------------------------------------------------------------------------------------
