void dateTime(){
  // The formattedDate comes with the following format:
  // 2018-05-28T16:00:13Z
  // We need to extract date and time
  formattedDate = timeClient.getFormattedDate();
  Serial.println(formattedDate);
  
  // Extract date
  
  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(0, splitT);
  Serial.print("DATE: ");
  Serial.println(dayStamp);
  
  // Extract time
  timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
  Serial.print("HOUR: ");
  Serial.println(timeStamp);
  
  timeWSec = formattedDate.substring(splitT+1, formattedDate.length()-4);
  Hour =formattedDate.substring(splitT+1, formattedDate.length()-7);
  Minute =formattedDate.substring(splitT+4, formattedDate.length()-4);
  Second =formattedDate.substring(splitT+7, formattedDate.length()-1);

  for(int i=0;i<5;i++){
  String H = "Data/set/setHour/H"+String(i); //Path H = Hour
  String M = "Data/set/setMinute/M"+String(i); //Path M = Minute  //   String S = "Data/set/setSecond/S"+String(i); //S = Second
  
   setHour[i] = TD32_Get_Firebase(H);
   setMinute[i] = TD32_Get_Firebase(M);  //   setSecond[i] = TD32_Get_Firebase(S);
   Set[i] = String(setHour[i])+":"+String(setMinute[i]);

  }
}
