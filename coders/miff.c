    while (isgraph(c) && (c != ':'))
    {
      register char
        *p;

      if (c == '{')
        {
          char
            *comment;

          /*
            Read comment-- any text between { }.
          */
          length=MaxTextExtent;
          comment=(char *) AllocateMemory(length);
          p=comment;
          for ( ; comment != (char *) NULL; p++)
          {
            c=ReadByte(image);
            if ((c == EOF) || (c == '}'))
              break;
            if ((p-comment+1) >= (int) length)
              {
                *p='\0';
                length<<=1;
                comment=(char *) ReallocateMemory(comment,length);
                if (comment == (char *) NULL)
                  break;
                p=comment+Extent(comment);
              }
            *p=c;
          }
          if (comment == (char *) NULL)
            ThrowReaderException(ResourceLimitWarning,
              "Memory allocation failed",image);
          *p='\0';
          (void) SetImageAttribute(image,"Comment",comment);
          FreeMemory((void **) &comment);
          c=ReadByte(image);
        }
      else
        if (isalnum(c))
          {
            /*
              Determine a keyword and its value.
            */
            p=keyword;
            do
            {
              if ((p-keyword) < (MaxTextExtent-1))
                *p++=c;
              c=ReadByte(image);
            } while (isalnum(c) || (c == '-'));
            *p='\0';
            while (isspace(c) || (c == '='))
              c=ReadByte(image);
            p=values;
            while ((c != '}') && (c != EOF))
            {
              if ((p-values+1) >= (int) length)
                {
                  *p='\0';
                  length<<=1;
                  values=(char *) ReallocateMemory(values,length);
                  if (values == (char *) NULL)
                    break;
                  p=values+Extent(values);
                }
              if (values == (char *) NULL)
                ThrowReaderException(ResourceLimitWarning,
                  "Memory allocation failed",image);
              *p++=c;
              c=ReadByte(image);
              if (*values != '{')
                if (isspace(c))
                  break;
            }
            *p='\0';
            /*
              Assign a value to the specified keyword.
            */
            switch (*keyword)
            {
              case 'b':
              case 'B':
              {
                if (LocaleCompare(keyword,"Background-color") == 0)
                  {
                    (void) QueryColorDatabase(values,&image->background_color);
                    break;
                  }
                if (LocaleCompare(keyword,"Blue-primary") == 0)
                  {
                    (void) sscanf(values,"%lf,%lf",
                      &image->chromaticity.blue_primary.x,
                      &image->chromaticity.blue_primary.y);
                    break;
                  }
                if (LocaleCompare(keyword,"Border-color") == 0)
                  {
                    (void) QueryColorDatabase(values,&image->border_color);
                    break;
                  }
                (void) SetImageAttribute(image,keyword,values);
                break;
              }
              case 'c':
              case 'C':
              {
                if (LocaleCompare(keyword,"Class") == 0)
                  {
                    if (LocaleCompare(values,"PseudoClass") == 0)
                      image->class=PseudoClass;
                    else
                      if (LocaleCompare(values,"DirectClass") == 0)
                        image->class=DirectClass;
                      else
                        image->class=UndefinedClass;
                    break;
                  }
                if (LocaleCompare(keyword,"Colors") == 0)
                  {
                    image->colors=(unsigned int) atoi(values);
                    break;
                  }
                if (LocaleCompare(keyword,"Color-profile") == 0)
                  {
                    image->color_profile.length=(unsigned int) atoi(values);
                    break;
                  }
                if (LocaleCompare(keyword,"Colorspace") == 0)
                  {
                    if (LocaleCompare(values,"CMYK") == 0)
                      image->colorspace=CMYKColorspace;
                    else
                      if (LocaleCompare(values,"RGB") == 0)
                        image->colorspace=RGBColorspace;
                    break;
                  }
                if (LocaleCompare(keyword,"Compression") == 0)
                  {
                    if (LocaleCompare(values,"Zip") == 0)
                      image->compression=ZipCompression;
                    else
                      if (LocaleCompare(values,"BZip") == 0)
                        image->compression=BZipCompression;
                      else
                        if (LocaleCompare(values,"RunlengthEncoded") == 0)
                          image->compression=RunlengthEncodedCompression;
                        else
                          image->compression=UndefinedCompression;
                    break;
                  }
                if (LocaleCompare(keyword,"Columns") == 0)
                  {
                    image->columns=(unsigned int) atoi(values);
                    break;
                  }
                (void) SetImageAttribute(image,keyword,values);
                break;
              }
              case 'd':
              case 'D':
              {
                if (LocaleCompare(keyword,"Delay") == 0)
                  {
                    if (image_info->delay == (char *) NULL)
                      image->delay=atoi(values);
                    break;
                  }
                if (LocaleCompare(keyword,"Depth") == 0)
                  {
                    image->depth=atoi(values) <= 8 ? 8 : 16;
                    break;
                  }
                if (LocaleCompare(keyword,"Dispose") == 0)
                  {
                    if (image_info->dispose == (char *) NULL)
                      image->dispose=atoi(values);
                    break;
                  }
                (void) SetImageAttribute(image,keyword,values);
                break;
              }
              case 'g':
              case 'G':
              {
                if (LocaleCompare(keyword,"Gamma") == 0)
                  {
                    image->gamma=atof(values);
                    break;
                  }
                if (LocaleCompare(keyword,"Green-primary") == 0)
                  {
                    (void) sscanf(values,"%lf,%lf",
                      &image->chromaticity.green_primary.x,
                      &image->chromaticity.green_primary.y);
                    break;
                  }
                (void) SetImageAttribute(image,keyword,values);
                break;
              }
              case 'i':
              case 'I':
              {
                if (LocaleCompare(keyword,"Id") == 0)
                  {
                    (void) strcpy(id,values);
                    break;
                  }
                if (LocaleCompare(keyword,"Iterations") == 0)
                  {
                    if (image_info->iterations == (char *) NULL)
                      image->iterations=atoi(values);
                    break;
                  }
                (void) SetImageAttribute(image,keyword,values);
                break;
              }
              case 'm':
              case 'M':
              {
                if (LocaleCompare(keyword,"Matte") == 0)
                  {
                    image->matte=(LocaleCompare(values,"True") == 0) ||
                      (LocaleCompare(values,"true") == 0);
                    break;
                  }
                if (LocaleCompare(keyword,"Matte-color") == 0)
                  {
                    (void) QueryColorDatabase(values,&image->matte_color);
                    break;
                  }
                if (LocaleCompare(keyword,"Montage") == 0)
                  {
                    (void) CloneString(&image->montage,values);
                    break;
                  }
                (void) SetImageAttribute(image,keyword,values);
                break;
              }
              case 'p':
              case 'P':
              {
                if (LocaleCompare(keyword,"Page") == 0)
                  {
                    ParseImageGeometry(PostscriptGeometry(values),
                      &image->page.x,&image->page.y,
                      &image->page.width,&image->page.height);
                    break;
                  }
                (void) SetImageAttribute(image,keyword,values);
                break;
              }
              case 'r':
              case 'R':
              {
                if (LocaleCompare(keyword,"Red-primary") == 0)
                  {
                    (void) sscanf(values,"%lf,%lf",
                      &image->chromaticity.red_primary.x,
                      &image->chromaticity.red_primary.y);
                    break;
                  }
                if (LocaleCompare(keyword,"Rendering-intent") == 0)
                  {
                    if (LocaleCompare(values,"Saturation") == 0)
                      image->rendering_intent=SaturationIntent;
                    else
                      if (LocaleCompare(values,"perceptual") == 0)
                        image->rendering_intent=PerceptualIntent;
                      else
                        if (LocaleCompare(values,"absolute") == 0)
                          image->rendering_intent=AbsoluteIntent;
                        else
                          if (LocaleCompare(values,"relative") == 0)
                            image->rendering_intent=RelativeIntent;
                          else
                            image->rendering_intent=UndefinedIntent;
                    break;
                  }
                if (LocaleCompare(keyword,"Resolution") == 0)
                  {
                    (void) sscanf(values,"%lfx%lf",&image->x_resolution,
                      &image->y_resolution);
                    break;
                  }
                if (LocaleCompare(keyword,"Rows") == 0)
                  {
                    image->rows=(unsigned int) atoi(values);
                    break;
                  }
                (void) SetImageAttribute(image,keyword,values);
                break;
              }
              case 's':
              case 'S':
              {
                if (LocaleCompare(keyword,"Scene") == 0)
                  {
                    image->scene=(unsigned int) atoi(values);
                    break;
                  }
                (void) SetImageAttribute(image,keyword,values);
                break;
              }
              case 'u':
              case 'U':
              {
                if (LocaleCompare(keyword,"Units") == 0)
                  {
                    if (LocaleCompare(values,"undefined") == 0)
                      image->units=UndefinedResolution;
                    else
                      if (LocaleCompare(values,"pixels-per-inch") == 0)
                        image->units=PixelsPerInchResolution;
                      else
                        if (LocaleCompare(values,"pixels-per-centimeter") == 0)
                          image->units=PixelsPerCentimeterResolution;
                    break;
                  }
                (void) SetImageAttribute(image,keyword,values);
                break;
              }
              case 'w':
              case 'W':
              {
                if (LocaleCompare(keyword,"White-point") == 0)
                  {
                    (void) sscanf(values,"%lf,%lf",
                      &image->chromaticity.white_point.x,
                      &image->chromaticity.white_point.y);
                    break;
                  }
                (void) SetImageAttribute(image,keyword,values);
                break;
              }
              case '{':
              {
                (void) SetImageAttribute(image,keyword,values+1);
                break;
              }
              default:
              {
                (void) SetImageAttribute(image,keyword,values);
                break;
              }
            }
          }
        else
          c=ReadByte(image);
      while (isspace(c))
        c=ReadByte(image);
    }
