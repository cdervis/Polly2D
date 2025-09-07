// TODO: Replace this by your company name and game name.
package com.MyCompany.MyGame;

import android.content.res.AssetManager;
import android.os.Bundle;
import org.libsdl.app.SDLActivity;

public class MainActivity extends SDLActivity {

  private AssetManager assetManager;

  @Override
  protected String[] getLibraries() {
    // TODO: Replace "MyGame" by the name of your game.
    return new String[] { "MyGame" };
  }

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    assetManager = getResources().getAssets();
    setAssetManager(assetManager);
  }

  private static native void setAssetManager(AssetManager assetManager);
}
