package org.bigroom.pcabpnnhandwrittendigitsrecog;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Bitmap.Config;
import android.graphics.Matrix;
import android.os.Bundle;
import android.os.Environment;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.ImageView;
import android.widget.TextView;

public class PatternRecognition extends Activity {
	ImageView imageShowView = null;
	TextView resulTextView = null;
	TextView timeTextView = null;
	Bitmap imgOrign = null;
	int w;
	int h;
	int[] pix = null;
	int[] resultInt = null;
	Bitmap resultImg = null;
	@SuppressLint("NewApi")
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		imageShowView = (ImageView) findViewById(R.id.myImageView);
		resulTextView = (TextView) findViewById(R.id.resultshow);
		timeTextView = (TextView) findViewById(R.id.time);
		// imgOrign=((BitmapDrawable)getResources().getDrawable(R.drawable.imagesource)).getBitmap();
		// imgOrign=BitmapFactory.decodeFile("/sdcard/PcaBpnnRecogTmpImg.jpg");
		String strCaptureFilePath = Environment.getExternalStorageDirectory()
				.getPath() + "/PcaBpnnRecogTmpImg.jpg";
		imgOrign = BitmapFactory.decodeFile(strCaptureFilePath);

		Log.d("rec",
				"bitmap width:" + imgOrign.getWidth() + ","
						+ imgOrign.getHeight());
		imgOrign = Bitmap.createScaledBitmap(imgOrign, 864, 480, true);// 处理成屏幕的比例
		imgOrign = Bitmap.createBitmap(imgOrign, 80, 40, 220, 340);// 按取景框比例截取图片
		imgOrign = adjustPhotoRotation(imgOrign, 90);
		Log.d("rec",
				"bitmap width:" + imgOrign.getWidth() + ","
						+ imgOrign.getHeight());

		imageShowView.setImageBitmap(imgOrign);
		// imageShowView.setRotation(90);
		w = 320;
		h = 200;
		pix = new int[w * h];
	}

	Bitmap adjustPhotoRotation(Bitmap bm, final int orientationDegree) {
		Matrix m = new Matrix();
		m.setRotate(orientationDegree, (float) bm.getWidth() / 2,
				(float) bm.getHeight() / 2);

		try {
			Bitmap bm1 = Bitmap.createBitmap(bm, 0, 0, bm.getWidth(),
					bm.getHeight(), m, true);
			return bm1;
		} catch (OutOfMemoryError ex) {
		}
		return null;

	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		menu.add(0, 2, 0, R.string.PcaBpnnRecog);
		menu.add(0, 3, 0, R.string.TakePic);
		menu.add(0, 4, 0, R.string.Exit);
		menu.add(0, 5, 0, R.string.toGray);
		menu.add(0, 6, 0, R.string.toBinary);
		menu.add(0, 7, 0, R.string.toBinaryOTSU);
		menu.add(0, 8, 0, R.string.GradientSharp);
		menu.add(0, 9, 0, R.string.RemoveScatterNoise);
		menu.add(0, 10, 0, R.string.CharSegment);
		return super.onCreateOptionsMenu(menu);
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		switch (item.getItemId()) {
			case 5 :
				// imgOrign.getPixels(pix, 0, w, 0, 50, w, h);
				imgOrign.getPixels(pix, 0, w, 0, 0, w, h);
				resultInt = ImgToGray(pix, w, h);
				resultImg = Bitmap.createBitmap(w, h, Config.RGB_565);
				resultImg.setPixels(resultInt, 0, w, 0, 0, w, h);
				imageShowView.setVisibility(ImageView.VISIBLE);
				imageShowView.setImageBitmap(resultImg);
				resulTextView.setText("");
				timeTextView.setText("");
				break;
			case 6 :
				imgOrign.getPixels(pix, 0, w, 0, 0, w, h);
				resultInt = ImgToBinary(pix, w, h);
				resultImg = Bitmap.createBitmap(w, h, Config.RGB_565);
				resultImg.setPixels(resultInt, 0, w, 0, 0, w, h);
				imageShowView.setImageBitmap(resultImg);
				resulTextView.setText("");
				timeTextView.setText("");
				break;
			case 7 :
				// imgOrign.getPixels(pix, 0, w, 0, 50, w, h);
				imgOrign.getPixels(pix, 0, w, 0, 0, w, h);
				resultInt = ImgToBinaryOTSU(pix, w, h);
				resultImg = Bitmap.createBitmap(w, h, Config.RGB_565);
				resultImg.setPixels(resultInt, 0, w, 0, 0, w, h);
				imageShowView.setImageBitmap(resultImg);
				resulTextView.setText("");
				timeTextView.setText("");
				break;
			case 8 :
				resultInt = GradientSharp(pix, w, h);
				resultImg = Bitmap.createBitmap(w, h, Config.RGB_565);
				resultImg.setPixels(resultInt, 0, w, 0, 0, w, h);
				imageShowView.setImageBitmap(resultImg);
				resulTextView.setText("");
				timeTextView.setText("");
				break;
			case 9 :
				// imgOrign.getPixels(resultInt1, 0, w, 0, 0, w, h);
				resultInt = RemoveScatterNoise(resultInt, w, h);
				resultImg = Bitmap.createBitmap(w, h, Config.RGB_565);
				resultImg.setPixels(resultInt, 0, w, 0, 0, w, h);
				imageShowView.setImageBitmap(resultImg);
				resulTextView.setText("");
				timeTextView.setText("");
				break;
			// case 4:
			// resultInt=SlopeAdjust(resultInt, w, h);
			// resultImg=Bitmap.createBitmap(w, h, Config.RGB_565);
			// resultImg.setPixels(resultInt, 0, w, 0, 0,w, h);
			// imageShowView.setImageBitmap(resultImg);
			// break;
			case 10 :
				resultInt = CharSegment(resultInt, w, h);
				resultImg = Bitmap.createBitmap(w, h, Config.RGB_565);
				resultImg.setPixels(resultInt, 0, w, 0, 0, w, h);
				imageShowView.setImageBitmap(resultImg);
				resulTextView.setText("");
				timeTextView.setText("");
				// SlopeAdjust(resultInt, w, h);
				break;
			case 0 :
				/*
				 * //imgOrign.getPixels(pix, 0, w, 0, 50, w, h);
				 * imgOrign.getPixels(pix, 0, w, 0, 240, w, h); int
				 * result[]=null; String tempString =""; long
				 * current=System.currentTimeMillis(); result=Segmentation(pix,
				 * w, h); long performance=System.currentTimeMillis()-current;
				 * for(int i=0;i<result.length;i++) {
				 * tempString+=String.valueOf(result[i]); }
				 * imageShowView.setVisibility(ImageView.GONE);
				 * resulTextView.setText(tempString);
				 * timeTextView.setText("Used "
				 * +String.valueOf(performance)+" milliseconds"); //
				 * resultInt=Segmentation(pix, w, h); //
				 * resultImg=Bitmap.createBitmap(w, h, Config.RGB_565); //
				 * resultImg.setPixels(resultInt, 0, w, 0, 0,w, h); //
				 * imageShowView.setImageBitmap(resultImg);
				 */
				break;
			case 1 :
				/*
				 * //imgOrign.getPixels(pix, 0, w, 0, 50, w, h);
				 * imgOrign.getPixels(pix, 0, w, 0, 240, w, h); int
				 * result1[]=null; String tempString1 =""; long
				 * current1=System.currentTimeMillis();
				 * result1=Segmentation2(pix, w, h); long
				 * performance1=System.currentTimeMillis()-current1; for(int
				 * i=0;i<result1.length;i++) {
				 * tempString1+=String.valueOf(result1[i]); }
				 * imageShowView.setVisibility(ImageView.GONE);
				 * resulTextView.setText(tempString1);
				 * timeTextView.setText("Used "
				 * +String.valueOf(performance1)+" milliseconds");
				 */
				break;
			case 2 :
				imgOrign.getPixels(pix, 0, w, 0, 0, w, h);
				int result2[] = null;
				String tempString2 = "";
				long current2 = System.currentTimeMillis();
				result2 = PcaBpnnRecog(pix, w, h);
				long performance2 = System.currentTimeMillis() - current2;
				for (int i = 0; i < result2.length; i++) {
					tempString2 += String.valueOf(result2[i]);
				}
				imageShowView.setVisibility(ImageView.GONE);
				resulTextView.setText(tempString2);
				timeTextView.setText("Used " + String.valueOf(performance2)
						+ " milliseconds");
				break;
			case 3 :
				Intent intent = new Intent();
				intent.setClass(PatternRecognition.this, MainActivity.class);
				startActivity(intent);
				PatternRecognition.this.finish();
			case 4 :
				this.finish();
		}
		return super.onOptionsItemSelected(item);
	}

	public static native int[] ImgToGray(int[] buf, int w, int h);
	public static native int[] ImgToBinary(int[] buf, int w, int h);
	public static native int[] ImgToBinaryOTSU(int[] buf, int w, int h);
	public static native int[] GradientSharp(int[] buf, int w, int h);
	public static native int[] RemoveScatterNoise(int[] buf, int w, int h);
	public static native int[] SlopeAdjust(int[] buf, int w, int h);
	public static native int[] CharSegment(int[] buf, int w, int h);
	public static native int[] Segmentation2(int[] buf, int w, int h);
	public static native int[] Segmentation(int[] buf, int w, int h);

	public static native int[] PcaBpnnRecog(int[] buf, int w, int h);

	static {
		// System.loadLibrary("PatternRecognition");
		// System.loadLibrary("Segmentation");
		// System.loadLibrary("Segmentation2");
		System.loadLibrary("PcaBpnnHandwrittenDigitsRecog");
	}
}
