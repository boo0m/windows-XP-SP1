//***********************    Localization variable    *****************
var L_strErrMsg_Message = "Invalid IP address entered."; 
//***********************    Localization variable    *****************

function loadForm()
// --------------------------------------------------------------------------------------
// function performed at page load; load the values stored in oDataStore
// --------------------------------------------------------------------------------------
{
	divMain2.load("oDataStore");
	var P_dc2=noNull(divMain2.getAttribute("P_dc2"));
	var P_dc3=noNull(divMain2.getAttribute("P_dc3"));
	P_dc3=""
	if(P_dc3!="")
	{
		var arr_dc3 = P_dc3.split(".");					
		for(i=0;i<form.txtDC.length;i++)
		{
			form.txtDC[i].value=arr_dc3[i];				
		}			
	}	
	else
	{
		var arr_dc2 = P_dc2.split(".");		
		for(i=0;i<=2;i++)
		{
			form.txtDC[i].value=arr_dc2[i];				
		}
		form.txtDC[3].value=(arr_dc2[3]<254)?parseInt(arr_dc2[3])+1:arr_dc2[3];	
		for(i=4;i<=6;i++)
		{
			form.txtDC[i].value=arr_dc2[i-4];				
		}		
	}
	loadFocus();
}
	
function accessKeys()
// --------------------------------------------------------------------------------------
// used Body.OnKeyDown 
// --------------------------------------------------------------------------------------
{
	key=event.keyCode;
	if(event.altKey)
	{	if(key==83) form.txtDC[0].focus();	// alt + s
		else if(key==69) form.txtDC[4].focus();	// alt + e			
	}			
}
	
function back()
// --------------------------------------------------------------------------------------
// process when click on BACK button (persist data and load previous dc.htm page)
// --------------------------------------------------------------------------------------
{
	if(checkForm2()) { saveForm(); self.location.href="dc2.htm"; }
	else showAlert(L_strErrMsg_Message);
}
	
function next()
// --------------------------------------------------------------------------------------
// process when NEXT button clicked ; check entries made and proceed to next page (pass.htm) 
// --------------------------------------------------------------------------------------
{
	if(checkForm2()) { saveForm(); self.location.href="pass.htm"; }
	else showAlert(L_strErrMsg_Message);
}

function saveForm()
// --------------------------------------------------------------------------------------
// process before exiting the page; saves the selection and entries made
// -------------------------------------------------------------------------------------
{
	var P_dc3="";
	for(i=0;i<form.txtDC.length;i++)
	{
		P_dc3 += form.txtDC[i].value;
		if(i!=form.txtDC.length-1) P_dc3 += ".";
	}		
	divMain2.setAttribute("P_dc3",P_dc3);
	divMain2.setAttribute("P_install","firstServer");
	divMain2.save("oDataStore");
}