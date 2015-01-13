package org.bigroom.pcabpnnhandwrittendigitsrecog;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Iterator;
import java.util.List;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.hardware.Camera;
import android.hardware.Camera.PictureCallback;
import android.hardware.Camera.ShutterCallback;
import android.hardware.Camera.Size;
import android.os.Bundle;
import android.os.Environment;
import android.text.Html;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup.LayoutParams;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;

public class MainActivity extends Activity {
	Preview mPreview;
	private Activity a;
	private Bundle b = new Bundle();

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
				WindowManager.LayoutParams.FLAG_FULLSCREEN);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		this.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
		a = this;
		mPreview = new Preview(this);
		DrawOnTop mDraw = new DrawOnTop(this);
		setContentView(mPreview);
		addContentView(mDraw, new LayoutParams(LayoutParams.WRAP_CONTENT,
				LayoutParams.WRAP_CONTENT));
		Button btn = new Button(a);
		btn.setText(Html.fromHtml("<b>拍照</b>"));
		addContentView(btn, new LayoutParams(LayoutParams.WRAP_CONTENT,
				LayoutParams.WRAP_CONTENT));
		btn.setOnClickListener(new OnClickListener(){

			@Override
			public void onClick(View v) {
				mPreview.mCamera.autoFocus(mPreview.mAutoFocusCallback);
			}
		});
	}
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		menu.add(0, 0, 0, R.string.Reset);
		menu.add(0, 1, 0, R.string.TakePic);
		menu.add(0, 2, 0, R.string.Recog);
		menu.add(0, 3, 0, R.string.Exit);
		return super.onCreateOptionsMenu(menu);
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		switch (item.getItemId()) {
			case 0 :
				mPreview.stopCamera();
				mPreview.initCamera();
				break;
			case 1 :
				mPreview.mCamera.autoFocus(mPreview.mAutoFocusCallback);
				break;
			case 2 :
				Intent intent = new Intent();
				intent.setClass(MainActivity.this, PatternRecognition.class);

				intent.putExtras(b);
				startActivity(intent);
				MainActivity.this.finish();
			case 3 :
				this.finish();
		}
		return super.onOptionsItemSelected(item);
	}

	class DrawOnTop extends View {
		Paint paint = new Paint();

		public DrawOnTop(Context context) {
			super(context);
		}

		@Override
		protected void onDraw(Canvas canvas) {

			paint.setStyle(Paint.Style.STROKE);
			paint.setColor(Color.GREEN);
			paint.setStrokeWidth(3);
			// canvas.drawRect(0, 100/3, 1600/3, 400/3, paint); //矩形框
			canvas.drawRect(60, 100, 420, 225, paint);

			b.putInt("left", 60);
			b.putInt("top", 60);
			b.putInt("right", 60);
			b.putInt("bottom", 60);
			super.onDraw(canvas);
		}
	}

	class Preview extends SurfaceView implements SurfaceHolder.Callback {
		public Camera mCamera;
		SurfaceHolder mHolder;
		private String strCaptureFilePath = Environment
				.getExternalStorageDirectory().getPath()
				+ "/PcaBpnnRecogTmpImg.jpg";
		public AutoFocusCallback mAutoFocusCallback = new AutoFocusCallback();

		public Preview(Context context) {
			super(context);
			mHolder = getHolder();
			mHolder.addCallback(this);
			mHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
		}

		@Override
		public void surfaceCreated(SurfaceHolder holder) {
			try {
				/* 打开相机， */
				mCamera = Camera.open();
				mCamera.setPreviewDisplay(holder);
			} catch (IOException exception) {
				mCamera.release();
				mCamera = null;
			}
		}

		@Override
		public void surfaceChanged(SurfaceHolder holder, int format, int width,
				int height) {
			/* 相机初始化 */
			initCamera();
		}

		@Override
		public void surfaceDestroyed(SurfaceHolder holder) {
			stopCamera();
			mCamera.release();
			mCamera = null;
		}

		public void takePicture() {
			if (mCamera != null) {
				mCamera.takePicture(shutterCallback, rawCallback, jpegCallback);
			}
		}

		private ShutterCallback shutterCallback = new ShutterCallback() {
			public void onShutter() {
			}
		};
		private PictureCallback rawCallback = new PictureCallback() {
			public void onPictureTaken(byte[] _data, Camera _camera) {
			}
		};
		private PictureCallback jpegCallback = new PictureCallback() {
			public void onPictureTaken(byte[] _data, Camera _camera) {
				Bitmap bm = BitmapFactory.decodeByteArray(_data, 0,
						_data.length);
				File myCaptureFile = new File(strCaptureFilePath);
				try {
					BufferedOutputStream bos = new BufferedOutputStream(
							new FileOutputStream(myCaptureFile));
					bm.compress(Bitmap.CompressFormat.JPEG, 80, bos);
					bos.flush();
					bos.close();
					stopCamera();
					// initCamera();
					a.finish();
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		};

		public final class AutoFocusCallback
				implements
					android.hardware.Camera.AutoFocusCallback {
			public void onAutoFocus(boolean focused, Camera camera) {
				/* 对到焦点拍照 */
				// if (focused) {
				takePicture();
				// }
			}
		};

		/* 相机初始化的method */
		public void initCamera() {
			if (mCamera != null) {
				try {
					// Camera.Parameters parameters = mCamera.getParameters();
					// /*
					// * 设定相片大小为1280*768， 格式为JPG
					// */
					// parameters.setPictureFormat(PixelFormat.JPEG);
					// //parameters.setPreviewSize(800, 480);
					// //parameters.setPictureSize(1280, 768);
					// parameters.setPreviewSize(1280, 720);
					// // parameters.setPictureSize(1280, 720);
					// mCamera.setParameters(parameters);
					setParameters(mCamera);
					/* 开启预览画面 */
					mCamera.startPreview();
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		}

		/* 停止相机的method */
		public void stopCamera() {
			if (mCamera != null) {
				try {
					/* 停止预览 */
					mCamera.stopPreview();
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		}

		private void setParameters(Camera camera) {
			Camera.Parameters parameters = camera.getParameters();
			List<Camera.Size> sizeList = parameters.getSupportedPreviewSizes();
			Log.d("rec", "preview size list:" + sizeList.toString());
			int previewWidth = 0;
			int previewHeight = 0;
			Iterator<Camera.Size> itor = sizeList.iterator();
			while (itor.hasNext()) {
				Camera.Size cur = itor.next();
				if (cur.width >= previewWidth && cur.height >= previewHeight) {
					previewWidth = cur.width;
					previewHeight = cur.height;
					break;
				}
			}

			parameters.setPreviewSize(previewWidth, previewHeight);
			Log.d("rec", "accepted preview:" + previewWidth + " "
					+ previewHeight);
			Size s = parameters.getSupportedPictureSizes().get(0);
			parameters.setPictureSize(s.width, s.height);
			Log.d("rec", "picturesize," + s.width + "," + s.height);
			setCameraDisplayOrientation(a, 0, camera);
			camera.setParameters(parameters);
		}

	}

	@SuppressLint("NewApi")
	public static void setCameraDisplayOrientation(Activity activity,
			int cameraId, Camera camera) {
		Camera.CameraInfo info = new android.hardware.Camera.CameraInfo();
		Camera.getCameraInfo(cameraId, info);
		int rotation = activity.getWindowManager().getDefaultDisplay()
				.getRotation();
		int degrees = 0;
		switch (rotation) {
			case Surface.ROTATION_0 :
				degrees = 0;
				break;
			case Surface.ROTATION_90 :
				degrees = 90;
				break;
			case Surface.ROTATION_180 :
				degrees = 180;
				break;
			case Surface.ROTATION_270 :
				degrees = 270;
				break;
		}

		int result;
		if (info.facing == Camera.CameraInfo.CAMERA_FACING_FRONT) {
			result = (info.orientation + degrees) % 360;
			result = (360 - result) % 360; // compensate the mirror
		} else { // back-facing
			result = (info.orientation - degrees + 360) % 360;
		}
		camera.setDisplayOrientation(result);
	}
}
