//  ------------------------------------------------------------------------------------------------
//  MODULE    : FicNom
//  LANGAGE   : C++
//  CREATEUR  : Adolfo VIDE
//  DATE    : October 15, 1992
//  DESCRIPTION : File name class
//  COMMENTAIRE :
//  SCCSID      : @(#)filename.h  1.1 11:50:00 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
//  ------------------------------------------------------------------------------------------------
  #ifndef FicNom_h
  #define FicNom_h
    #include "common_a.h"
//  ------------------------------------------------------------------------------------------------

//  Includes
//  --------

#ifdef macintosh
  #include  <StandardFile.h>
  #include  <Files.h>
#endif
  #include  "b_string.h"

//  Constantes
//  ----------

  // Search criteria used in SearchWithCriteria()
  enum searchCriteria {
    searchOnName,     // search files with same name
    searchOnType,     // search files with same type
    searchSubFolder     // search sub-folders
  };
  
//  Declarations des classes
//  ------------------------

class FicNom;
typedef FicNom* ptr_FicNom;
typedef FicNom& ref_FicNom;
  
#ifdef macintosh
  struct MemoDirVol;
  typedef MemoDirVol* ptr_MemoDirVol;
  typedef MemoDirVol& ref_MemoDirVol;
#endif

//  Definitions des classes
//  -----------------------

typedef OSErr (* FicNom_SearchFunc) (FicNom* fileName);


class FicNom {
  public:
                FicNom();
                ~FicNom();
                FicNom(const FicNom& fic);
  #ifdef macintosh
                FicNom(const FSSpec& specs);
  #endif
                FicNom(const ChaineFic& fileName,
                     OSType fileType = 0, short fileVolume = 0, long fileDirectory = 0);
                FicNom(const char* fileName, OSType fileType = 0);
                FicNom(const FicNom& fic, short newVolume);

        // To know if there is at least size KBytes on the disk
        // If not, write an error message (on Macintosh)
        Boolean     EnoughFreeSpace(long sizeInKBytes) const;
        // Give the free space in *this's volume (in KBytes)
        long      PlaceDisponible() const;

        OSErr     Detruit() const;
        OSErr     Search();
        OSErr     Exist() const;
        OSErr     CheckNameWithPeriod(Boolean withDialog) const;  // check if the name begin with '.'
        // Get the size of the file (in KBytes)
        OSErr     GetSize(long* sizeInKBytes);
        // look if the file is empty
        OSErr     Empty(Boolean* empty);

#ifdef macintosh
        // Les recherches s'effectuent seulement sur le nom du fichier
        OSErr     ChercheDsVolume(short volume, FSSpec* tableauDeFSSpec, long* nbReponse);
        OSErr     ChercheDsTsVolumes(FicNom* tableauDeFichier, long* nbReponse);
        
        OSErr     ChercheDsRepertoire(FicNom* tableauDeFichier, long* nbReponse, ptr_FicNom sourceFolder);
        OSErr     ChercheDsVolumesLocaux(FicNom* tableauDeFichier, long* nbReponse);
        OSErr     ChercheDsVolumesDistants(FicNom* tableauDeFichier, long* nbReponse);
        OSErr     SearchSameType(FicNom* tableauDeFichier, long* nbReponse, ptr_FicNom sourceFolder = NULL);
        OSErr     SearchSameTypeInFolder(FicNom* tableauDeFichier, long* nbReponse);
        OSErr     GiveNumberOfHierarchicalLevels(long* nbNiveauxPtr) const;
        OSErr     ConstructTruncPathName(short width, TruncCode truncWhere, Chaine * chaineText) const;
                                  // construct the associated path name
        unsigned long DateModification();
        unsigned long DateCreation();
        void      GetNewLocation ();

        void      SetToDirectoryChargement();
        void      SetToDirectorySauvegarde();
        Boolean     ChangeFolder(const FicNom& folder);
        void      SetDefaultFolder();
#endif
        
    friend  inline  int   operator ==(const FicNom& fic1, const FicNom& fic2);
    friend  inline  int   operator !=(const FicNom& fic1, const FicNom& fic2);
        void      CopyToStr255 (Str255 fic) const;
        OSErr     Copy(const FicNom& newFileName);
        OSErr     Rename(const FicNom& newFileName);
        OSErr     Move(const FicNom& newFileName);

#ifdef macintosh
        void      CopyToFSSpec (FSSpec& fic) const;
#endif
        void      CopyToCStr (char* fic) const;

        ChaineFic   nom;
        OSType      type;
        long      directory;
        short     volume;
    
    static  Boolean       showErrorDialog;
    static  FicNom_SearchFunc searchFunction;       
  
  protected:
        void      DirectoryDeSauvegarde();
        void      DirectoryDeChargement();
        void      MemorisationDeSauvegarde();
        void      MemorisationDeChargement();
#ifdef macintosh
        OSErr     SearchWithCriteria (FSSpec* tableauDeFSSpec, long* nbReponse, searchCriteria what = searchOnType);
        OSErr     SearchInFolder (FSSpec* tableauDeFSSpec, long* nbReponse, ptr_FicNom sourceFolder, searchCriteria what = searchOnType, Boolean into = true);
      
        ptr_MemoDirVol  Recherche(ptr_MemoDirVol memo);
#endif
  
  };
  



  
#ifdef macintosh
  struct MemoDirVol {
  
                MemoDirVol();
                MemoDirVol(const FicNom& fic, ptr_MemoDirVol avant);
                
        OSType      type;
        long      directory;
        short     volume;
        ptr_MemoDirVol  suivant;
  };
#endif




//  Fonctions 'inline'
//  ------------------

  inline FicNom::~FicNom() { }

  inline void FicNom::CopyToStr255 (Str255 fic) const
  
  {
    BlockMove (nom.Texte()-1, fic, nom.Longueur()+1);
  }

  inline void FicNom::CopyToCStr (char* fic) const
  
  {
    BlockMove (nom.Texte(), fic, nom.Longueur());
    *(fic+nom.Longueur()) = '\0';
  }

#ifdef macintosh
  inline void FicNom::CopyToFSSpec (FSSpec& fic) const
  
  {
    fic.vRefNum = volume;
    fic.parID = directory;
    BlockMove (nom.Texte()-1, &fic.name, nom.Longueur()+1);
  }
#endif


  inline FicNom::FicNom()
  
  {
    nom = ChaineFic(CStringHolder(""));;
    type = 0;
    directory = 0;
    volume = 0;
  }
  
  inline int operator ==(const FicNom& fic1, const FicNom& fic2)
  
  {
#ifdef macintosh
    return  (fic1.volume == fic2.volume) &&
        (fic1.directory == fic2.directory) &&
        (fic1.type == fic2.type) &&
        (fic1.nom == fic2.nom);
#else
    return  (fic1.nom == fic2.nom);
#endif
  }
  
  inline int operator !=(const FicNom& fic1, const FicNom& fic2)
  
  {
#ifdef macintosh
    return  (fic1.volume != fic2.volume) ||
        (fic1.directory != fic2.directory) ||
        (fic1.type != fic2.type) ||
        (fic1.nom != fic2.nom);
#else
    return  (fic1.nom != fic2.nom);
#endif
  }
  
#ifdef macintosh
  inline MemoDirVol::MemoDirVol()
  
  {
  }
#endif
  
//  Fonctions 'extern'
//  ------------------

#ifdef macintosh
// Get the current program name : used by the
  extern OSErr GetProgramName (ref_FicNom programName);
#endif

//  Variables 'extern'
//  ------------------

#ifdef macintosh
  extern ptr_MemoDirVol enLecture;
  extern ptr_MemoDirVol enEcriture;
#endif
  
//  ------------------------------------------------------------------------------------------------
  #endif // FicNom_h
//  ------------------------------------------------------------------------------------------------
