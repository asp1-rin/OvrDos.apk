package com.sys.opt;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.view.WindowManager;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;
import java.io.BufferedReader;
import java.io.InputStreamReader;

public class MainActivity extends Activity {

    // C++ 네이티브 라이브러리 로드 (메모리, 바이패스, 에임봇 통합 빌드본)
    static {
        System.loadLibrary("sys_core");
    }

    // C++ 함수 선언
    public native void startBypass(int pid);
    public native void tickAimbot(int pid, long gameBase, float fovRadius, int width, int height);

    private int targetPid = -1;
    private long gameModuleBase = 0;
    private float currentFovRadius = 150f; // 기본 반지름 초기값
    private boolean isRunning = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        // 메인 레이아웃 설정 (슬라이더가 포함된 XML 필요)
        setContentView(R.layout.activity_main);

        final TextView fovText = findViewById(R.id.fov_status_text);
        SeekBar fovSlider = findViewById(R.id.fov_slider);

        // 1. 게임 프로세스 감지
        targetPid = getProcessId("com.target.game"); // 실제 게임 패키지명으로 변경 필요

        if (targetPid != -1) {
            // 2. 보안 우회 실행 (Bypass)
            startBypass(targetPid);
            Toast.makeText(this, "OvrDos: Bypass Applied", Toast.LENGTH_SHORT).show();
            isRunning = true;
            
            // 에임봇 스레드 시작
            startAimbotEngine();
        } else {
            Toast.makeText(this, "게임이 실행 중이지 않습니다.", Toast.LENGTH_LONG).show();
        }

        // 3. 슬라이더 이벤트 처리 (반지름 조절)
        fovSlider.setMax(500); // 최대 반지름 500
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
    }

    // 에임봇 무한 루프 (별도 스레드에서 실행하여 앱 버벅임 방지)
    private void startAimbotEngine() {
        new Thread(() -> {
            // 실전에서는 모듈 베이스 주소를 찾는 로직이 필요함
            // 여기서는 예시 주소를 넣거나 C++ 내부에서 찾도록 설계
            gameModuleBase = 0x7000000000L; 

            while (isRunning) {
                if (targetPid != -1) {
                    // 화면 크기 획득 (FOV 계산용)
                    WindowManager wm = (WindowManager) getSystemService(Context.WINDOW_SERVICE);
                    int width = wm.getDefaultDisplay().getWidth();
                    int height = wm.getDefaultDisplay().getHeight();

                    // C++ 에임봇 엔진 호출
                    tickAimbot(targetPid, gameModuleBase, currentFovRadius, width, height);
                }
                
                try {
                    // CPU 과부하 방지를 위한 미세한 휴식 (대략 100FPS 수준)
                    Thread.sleep(10); 
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }).start();
    }

    // 루팅 권한을 이용한 PID 검색 유틸리티
    private int getProcessId(String packageName) {
        try {
            Process p = Runtime.getRuntime().exec("pidof " + packageName);
            BufferedReader br = new BufferedReader(new InputStreamReader(p.getInputStream()));
            String line = br.readLine();
            if (line != null && !line.isEmpty()) {
                return Integer.parseInt(line.trim().split(" ")[0]);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return -1;
    }
}
