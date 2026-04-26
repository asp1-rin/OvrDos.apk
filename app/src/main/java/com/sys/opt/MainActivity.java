package com.sys.opt;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.view.WindowManager;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;
import java.io.BufferedReader;
import java.io.InputStreamReader;

public class MainActivity extends Activity {

    static {
        System.loadLibrary("sys_core");
    }

    // C++ 네이티브 함수 연결
    public native void startBypass(int pid);
    public native void tickAimbot(int pid, long gameBase, float fovRadius, float smooth, int width, int height);

    private int targetPid = -1;
    private float currentFovRadius = 150f; 
    private float currentSmooth = 1.0f; // 기본값 1.0 (빡고)
    private boolean isRunning = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        final TextView fovText = findViewById(R.id.fov_status_text);
        final TextView smoothText = findViewById(R.id.smooth_status_text);
        SeekBar fovSlider = findViewById(R.id.fov_slider);
        SeekBar smoothSlider = findViewById(R.id.smooth_slider);

        // 1. 게임 프로세스 감지 (실제 패키지명으로 변경 필요)
        targetPid = getProcessId("com.target.game"); 

        if (targetPid != -1) {
            startBypass(targetPid);
            Toast.makeText(this, "OvrDos: Bypass & Hook Active", Toast.LENGTH_SHORT).show();
            isRunning = true;
            startAimbotEngine();
        }

        // 2. FOV 슬라이더 설정
        fovSlider.setMax(500);
        fovSlider.setProgress((int)currentFovRadius);
        fovSlider.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                currentFovRadius = (float) progress;
                fovText.setText("FOV Radius: " + progress);
            }
            @Override public void onStartTrackingTouch(SeekBar seekBar) {}
            @Override public void onStopTrackingTouch(SeekBar seekBar) {}
        });

        // 3. Smooth(속도) 슬라이더 설정
        smoothSlider.setMax(50); // 1(빡고) ~ 50(매우 부드러움)
        smoothSlider.setProgress((int)currentSmooth);
        smoothSlider.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                // 0일 경우 나누기 오류 방지를 위해 최소값 1.0 설정
                currentSmooth = (progress < 1) ? 1.0f : (float)progress;
                smoothText.setText("Smoothing: " + currentSmooth + (currentSmooth == 1.0f ? " (Snap)" : " (Legit)"));
            }
            @Override public void onStartTrackingTouch(SeekBar seekBar) {}
            @Override public void onStopTrackingTouch(SeekBar seekBar) {}
        });
    }

    private void startAimbotEngine() {
        new Thread(() -> {
            long gameModuleBase = 0x7000000000L; // 실제 구현 시 메모리에서 주소 탐색 필요
            while (isRunning) {
                if (targetPid != -1) {
                    WindowManager wm = (WindowManager) getSystemService(Context.WINDOW_SERVICE);
                    int width = wm.getDefaultDisplay().getWidth();
                    int height = wm.getDefaultDisplay().getHeight();

                    // C++ 엔진 호출 (PID, 베이스, FOV, Smooth, 화면가로, 화면세로)
                    tickAimbot(targetPid, gameModuleBase, currentFovRadius, currentSmooth, width, height);
                }
                try { Thread.sleep(10); } catch (Exception e) {}
            }
        }).start();
    }

    private int getProcessId(String packageName) {
        try {
            Process p = Runtime.getRuntime().exec("pidof " + packageName);
            BufferedReader br = new BufferedReader(new InputStreamReader(p.getInputStream()));
            String line = br.readLine();
            if (line != null && !line.isEmpty()) return Integer.parseInt(line.trim().split(" ")[0]);
        } catch (Exception e) {}
        return -1;
    }
}
