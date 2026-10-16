package com.embervfx.studio;

import android.app.Activity;
import android.content.Intent;
import android.content.pm.ApplicationInfo;
import android.graphics.Color;
import android.net.Uri;
import android.os.Bundle;
import android.util.Base64;
import android.util.Log;
import android.view.View;
import android.webkit.*;
import java.io.*;
import java.nio.charset.StandardCharsets;
import java.util.*;
import org.json.JSONObject;

public final class MainActivity extends Activity {
  private static final String HOST="appassets.androidplatform.net", START="https://"+HOST+"/assets/index.html";
  private static final int OPEN=1, SAVE=2, PICK_IMAGE=3, LIMIT=16*1024*1024;
  private WebView web;
  private String pendingId;
  private byte[] pendingBytes;
  private ValueCallback<Uri[]> imageCallback;
  private boolean debug;
  @Override public void onCreate(Bundle state) {
    super.onCreate(state);
    debug=(getApplicationInfo().flags&ApplicationInfo.FLAG_DEBUGGABLE)!=0;
    web=new WebView(this);web.setBackgroundColor(Color.rgb(20,19,26));android.widget.FrameLayout container=new android.widget.FrameLayout(this);container.setBackgroundColor(Color.rgb(23,21,30));container.addView(web,new android.widget.FrameLayout.LayoutParams(-1,-1));setContentView(container);
    // Keep controls inside system bars, including Android versions that enforce edge-to-edge.
    container.setOnApplyWindowInsetsListener((v,insets)->{if(android.os.Build.VERSION.SDK_INT>=30){android.graphics.Insets bars=insets.getInsets(android.view.WindowInsets.Type.systemBars()|android.view.WindowInsets.Type.displayCutout());v.setPadding(bars.left,bars.top,bars.right,bars.bottom);}else v.setPadding(insets.getSystemWindowInsetLeft(),insets.getSystemWindowInsetTop(),insets.getSystemWindowInsetRight(),insets.getSystemWindowInsetBottom());return insets;});
    container.requestApplyInsets();
    WebSettings s=web.getSettings();s.setJavaScriptEnabled(true);s.setDomStorageEnabled(true);s.setAllowFileAccess(false);s.setAllowContentAccess(false);s.setMixedContentMode(WebSettings.MIXED_CONTENT_NEVER_ALLOW);s.setMediaPlaybackRequiresUserGesture(true);
    WebView.setWebContentsDebuggingEnabled(debug);
    web.addJavascriptInterface(new Bridge(),"EmberAndroid");
    web.setWebViewClient(new WebViewClient(){
      @Override public boolean shouldOverrideUrlLoading(WebView view,WebResourceRequest req){return !local(req.getUrl());}
      @Override public WebResourceResponse shouldInterceptRequest(WebView view,WebResourceRequest req){
        Uri uri=req.getUrl();String path=uri.getPath();
        if(!local(uri)||path==null||!path.startsWith("/assets/")||path.contains("..")||path.contains("\\"))return blocked();
        try {String name=path.substring(8);Map<String,String> headers=new HashMap<>();headers.put("Cache-Control","no-cache");headers.put("X-Content-Type-Options","nosniff");headers.put("Content-Security-Policy","default-src 'self' data: blob:; script-src 'self' 'unsafe-inline'; style-src 'self' 'unsafe-inline'; connect-src 'self' blob:; frame-src 'none'; object-src 'none'");return new WebResourceResponse(mime(name),"UTF-8",200,"OK",headers,getAssets().open(name));}
        catch(IOException e){return blocked();}
      }
    });
    web.setWebChromeClient(new WebChromeClient(){
      @Override public boolean onConsoleMessage(ConsoleMessage m){if(debug)Log.i("EmberQA",m.message());return true;}
      @Override public boolean onShowFileChooser(WebView view,ValueCallback<Uri[]> callback,FileChooserParams params){
        if(imageCallback!=null||pendingId!=null){callback.onReceiveValue(null);return true;}
        imageCallback=callback;
        Intent i=new Intent(Intent.ACTION_OPEN_DOCUMENT).addCategory(Intent.CATEGORY_OPENABLE).setType("image/png");
        try{startActivityForResult(i,PICK_IMAGE);}catch(Exception e){imageCallback=null;callback.onReceiveValue(null);}return true;
      }
    });
    web.loadUrl(START);
    if(android.os.Build.VERSION.SDK_INT>=33)getOnBackInvokedDispatcher().registerOnBackInvokedCallback(android.window.OnBackInvokedDispatcher.PRIORITY_DEFAULT,()->handleBack());
  }
  private boolean local(Uri uri){return "https".equals(uri.getScheme())&&HOST.equals(uri.getHost())&&uri.getPort()==-1;}
  private WebResourceResponse blocked(){return new WebResourceResponse("text/plain","UTF-8",403,"Blocked",Collections.emptyMap(),new ByteArrayInputStream(new byte[0]));}
  private String mime(String path){if(path.endsWith(".html"))return "text/html";if(path.endsWith(".js"))return "text/javascript";if(path.endsWith(".css"))return "text/css";if(path.endsWith(".svg"))return "image/svg+xml";if(path.endsWith(".png"))return "image/png";if(path.endsWith(".json"))return "application/json";if(path.endsWith(".woff2"))return "font/woff2";return "application/octet-stream";}
  private JSONObject result(boolean ok,boolean canceled,String error){JSONObject r=new JSONObject();try{r.put("ok",ok);r.put("canceled",canceled);if(error!=null)r.put("error",error);}catch(Exception ignored){}return r;}
  private void reply(String id,JSONObject value){if(web!=null)web.evaluateJavascript("window.emberAndroidReply("+JSONObject.quote(id)+","+value.toString()+")",null);}
  private final class Bridge {
    @JavascriptInterface public void request(String raw){
      if(raw==null||raw.length()>24*1024*1024)return;
      runOnUiThread(()->{String id="";try{
        if(!START.equals(web.getUrl()))throw new IOException("Only Ember can access project files");
        JSONObject req=new JSONObject(raw);id=req.getString("id");if(!id.matches("[0-9]{1,10}"))return;
        if(pendingId!=null||imageCallback!=null){reply(id,result(false,false,"Finish the current file picker first."));return;}
        String action=req.getString("action");Intent intent;
        if(action.equals("open")){intent=new Intent(Intent.ACTION_OPEN_DOCUMENT).addCategory(Intent.CATEGORY_OPENABLE).setType("*/*");}
        else if(action.equals("save")){
          String type=req.getString("mime");if(!type.equals("application/json")&&!type.equals("image/png"))throw new IOException("Unsupported file type");
          pendingBytes=Base64.decode(req.getString("base64"),Base64.DEFAULT);if(pendingBytes.length>LIMIT)throw new IOException("File is too large");
          String name=req.optString("name","effect.ember.json").replaceAll("[^A-Za-z0-9._ -]","-");if(name.length()>120)name=name.substring(0,120);
          intent=new Intent(Intent.ACTION_CREATE_DOCUMENT).addCategory(Intent.CATEGORY_OPENABLE).setType(type).putExtra(Intent.EXTRA_TITLE,name);
        }else throw new IOException("Unknown file action");
        pendingId=id;startActivityForResult(intent,action.equals("open")?OPEN:SAVE);
      }catch(Exception e){pendingId=null;pendingBytes=null;if(!id.isEmpty())reply(id,result(false,false,e.getMessage()));}});
    }
  }
  @Override protected void onActivityResult(int request,int code,Intent data){
    super.onActivityResult(request,code,data);
    if(request==PICK_IMAGE){if(imageCallback!=null){imageCallback.onReceiveValue(code==RESULT_OK&&data!=null&&data.getData()!=null?new Uri[]{data.getData()}:null);imageCallback=null;}return;}
    if(request!=OPEN&&request!=SAVE)return;
    String id=pendingId;byte[] bytes=pendingBytes;pendingId=null;pendingBytes=null;if(id==null)return;
    if(code!=RESULT_OK||data==null||data.getData()==null){reply(id,result(true,true,null));return;}
    Uri uri=data.getData();
    new Thread(()->{JSONObject response;try{
      if(request==SAVE){try(OutputStream out=getContentResolver().openOutputStream(uri,"wt")){if(out==null)throw new IOException("Cannot write this file");out.write(bytes);}response=result(true,false,null);}
      else {ByteArrayOutputStream buffer=new ByteArrayOutputStream();try(InputStream in=getContentResolver().openInputStream(uri)){if(in==null)throw new IOException("Cannot read this file");byte[] chunk=new byte[8192];int n;while((n=in.read(chunk))!=-1){if(buffer.size()+n>LIMIT)throw new IOException("Project is too large (16 MB maximum)");buffer.write(chunk,0,n);}}response=result(true,false,null);response.put("text",buffer.toString("UTF-8"));}
    }catch(Exception e){response=result(false,false,e.getMessage());}final JSONObject r=response;runOnUiThread(()->reply(id,r));},"EmberFiles").start();
  }
  @Override protected void onPause(){if(web!=null)web.evaluateJavascript("window.emberMobilePause&&window.emberMobilePause()",null);super.onPause();if(web!=null)web.onPause();}
  @Override protected void onResume(){super.onResume();if(web!=null){web.onResume();web.evaluateJavascript("window.emberMobileResume&&window.emberMobileResume()",null);}}
  private void handleBack(){web.evaluateJavascript("window.emberMobileBack&&window.emberMobileBack()",value->{if(!"true".equals(value))moveTaskToBack(true);});}
  @Override public void onBackPressed(){handleBack();}
  @Override protected void onDestroy(){if(imageCallback!=null)imageCallback.onReceiveValue(null);if(web!=null){web.removeJavascriptInterface("EmberAndroid");web.destroy();web=null;}super.onDestroy();}
}
