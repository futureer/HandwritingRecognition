package org.bigroom.pcabpnnhandwrittendigitsrecog;

import java.util.ArrayList;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

public class PatternRecognition extends Activity implements OnClickListener {
	ImageView imageShowView = null;
	TextView resulTextView = null;
	TextView timeTextView = null;
	Bitmap imgOrign = null;
	int w;
	int h;
	int[] pix = null;
	int[] resultInt = null;
	private Bitmap resultImg = null;
	private Button takePicBtn, resetBtn,recogBtn;
	private LinearLayout btnLine, btnLine1, btnLine2;

	@SuppressLint("NewApi")
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		resulTextView = (TextView) findViewById(R.id.resultshow);
		timeTextView = (TextView) findViewById(R.id.time);
		takePicBtn = (Button) findViewById(R.id.take_pic);
		resetBtn = (Button) findViewById(R.id.reset_btn);
		recogBtn = (Button) findViewById(R.id.recogintion_btn);
		recogBtn.setOnClickListener(this);
		takePicBtn.setOnClickListener(this);
		resetBtn.setOnClickListener(this);
		btnLine = (LinearLayout) findViewById(R.id.btn_line);
		btnLine1 = (LinearLayout) findViewById(R.id.btn_line1);
		btnLine2 = (LinearLayout) findViewById(R.id.btn_line2);
		loadBtns();
		imageShowView = (ImageView) findViewById(R.id.myImageView);
		preloadBitmap();
	}

	String options[] = new String[]{"Gray", "Binaryzation", "OTSUBinaryzation",
			"GradientSharp", "RemoveScatterNoise", "Segmentation"};
	private void loadBtns() {
		ArrayList<Button> btns = new ArrayList<Button>(6);
		for (int i = 0; i < 6; i++) {
			Button btn = new Button(this);
			btn.setTag(i);
			btn.setOnClickListener(this);
			btn.setText(options[i]);
			btns.add(btn);
			if (i < 2)
				btnLine.addView(btn, i);
			else if (i < 4)
				btnLine1.addView(btn, i - 2);
			else if (i < 6)
				btnLine2.addView(btn, i - 4);
		}

	}
	private void preloadBitmap() {
		String strCaptureFilePath = Environment.getExternalStorageDirectory()
				.getPath() + "/PcaBpnnRecogTmpImg.jpg";
		imgOrign = BitmapFactory.decodeFile(strCaptureFilePath);
		if (imgOrign == null) {
			resulTextView.setText("take a picture first!");
		}
		Log.d("rec",
				"bitmap width:" + imgOrign.getWidth() + ","
						+ imgOrign.getHeight());
		imgOrign = Bitmap.createScaledBitmap(imgOrign, 864, 480, true);// 处理成屏幕的比例
		imgOrign = adjustPhotoRotation(imgOrign, 90);
		imgOrign = Bitmap.createBitmap(imgOrign, 80, 100, 320, 150);// 按取景框比例截取图片
		Log.d("rec",
				"bitmap width:" + imgOrign.getWidth() + ","
						+ imgOrign.getHeight());

		imageShowView.setImageBitmap(imgOrign);
		w = 320;
		h = 150;
		pix = new int[w * h];
	}

	public static Bitmap adjustPhotoRotation(Bitmap bm,
			final int orientationDegree) {
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

	@Override
	public void onClick(View v) {
		switch (v.getId()) {
			case R.id.take_pic :
				Intent in = new Intent(this, MainActivity.class);
				startActivity(in);
				break;
			case R.id.reset_btn :
				preloadBitmap();
				break;
			case R.id.recogintion_btn :
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
		}

		int tag = 100;
		if (null != (Integer) v.getTag()) {
			tag = (Integer) v.getTag();
		}
		switch (tag) {
			case 0 :// Gray
				Log.d("rec", ((Button) v).getText().toString());
				imgOrign.getPixels(pix, 0, w, 0, 0, w, h);
				resultInt = ImgToGray(pix, w, h);
				resultImg = Bitmap.createBitmap(w, h, Config.RGB_565);
				resultImg.setPixels(resultInt, 0, w, 0, 0, w, h);
				imageShowView.setVisibility(ImageView.VISIBLE);
				imageShowView.setImageBitmap(resultImg);
				break;
			case 1 :// Binaryzation
				Log.d("rec", ((Button) v).getText().toString());
				imgOrign.getPixels(pix, 0, w, 0, 0, w, h);
				resultInt = ImgToBinary(pix, w, h);
				resultImg = Bitmap.createBitmap(w, h, Config.RGB_565);
				resultImg.setPixels(resultInt, 0, w, 0, 0, w, h);
				imageShowView.setImageBitmap(resultImg);
				break;
			case 2 :// OTSUBinaryzation
				Log.d("rec", ((Button) v).getText().toString());
				imgOrign.getPixels(pix, 0, w, 0, 0, w, h);
				resultInt = ImgToBinaryOTSU(pix, w, h);
				resultImg = Bitmap.createBitmap(w, h, Config.RGB_565);
				resultImg.setPixels(resultInt, 0, w, 0, 0, w, h);
				imageShowView.setImageBitmap(resultImg);
				break;
			case 3 :// GradientSharp
				Log.d("rec", ((Button) v).getText().toString());
				resultInt = GradientSharp(pix, w, h);
				resultImg = Bitmap.createBitmap(w, h, Config.RGB_565);
				resultImg.setPixels(resultInt, 0, w, 0, 0, w, h);
				imageShowView.setImageBitmap(resultImg);
				break;
			case 4 :// RemoveScatterNoise
				Log.d("rec", ((Button) v).getText().toString());
				resultInt = RemoveScatterNoise(resultInt, w, h);
				resultImg = Bitmap.createBitmap(w, h, Config.RGB_565);
				resultImg.setPixels(resultInt, 0, w, 0, 0, w, h);
				imageShowView.setImageBitmap(resultImg);
				break;
			case 5 :// Segmentation
				Log.d("rec", ((Button) v).getText().toString());
				resultInt = CharSegment(resultInt, w, h);
				resultImg = Bitmap.createBitmap(w, h, Config.RGB_565);
				resultImg.setPixels(resultInt, 0, w, 0, 0, w, h);
				imageShowView.setImageBitmap(resultImg);
				break;
		}
	}
}
