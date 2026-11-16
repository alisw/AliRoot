void MergeESDTags()
{
  char spath[2048];
  char sglob[1024];
  snprintf(spath,2048"");
  snprintf(sglob,1024"");
  for (int i=0; i< gApplication->Argc();i++){
    if (!(strcmp(gApplication->Argv(i),"--path")))
      snprintf(spath, 2048, gApplication->Argv(i+1));
    if (!(strcmp(gApplication->Argv(i),"--glob")))
      snprintf(sglob, 1024, gApplication->Argv(i+1));
  }

  if (!strcmp(sglob, "")) snprintf(sglob,1024,"ESD.tag.root");

  printf("*** Connect to AliEn ***\n");
  TGrid::Connect("alien://");
  gSystem->Load("libANALYSIS");
  gSystem->Load("libANALYSISalice");
  
  // Create A tag creator object 
  AliESDTagCreator *tagCre = new AliESDTagCreator();
  tagCre->SetStorage(0);

  // Find all the event tag files in the GRID directory
  TGridResult* tagResult = gGrid->Query(spath,"ESD.tag.root");

  // Merge the tags	
  tagCre->MergeTagsForRun("ESD",tagResult);

  return;
}
