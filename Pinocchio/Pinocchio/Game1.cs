using System;
using System.Collections.Generic;
using System.Linq;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Audio;
using Microsoft.Xna.Framework.Content;
using Microsoft.Xna.Framework.GamerServices;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Input;
using Microsoft.Xna.Framework.Media;
using System.Windows.Forms;
using Primitives3D;
using Pinocchio.Model;
using Pinocchio.animation3D;
using Pinocchio.Camera;
using Microsoft.Kinect;
using System.Collections;
using System.IO;

namespace Pinocchio
{
    /// <summary>
    /// This is the main type for your game
    /// </summary>
    public class Game1 : Microsoft.Xna.Framework.Game
    {
        GraphicsDeviceManager graphics;
        SpriteBatch spriteBatch;
        IntPtr m_WindowHandle;
        double screenWidth;
        double screenHeight;

        private ArrayList kinects;
        private KinectSensor sensor;    // TODO 현재는 하나의 키넥트만 활용
        private short[] depthPixels;
        private short[] copyOfDepthPixels;

        int remainderTime = 0;      // deltaFrame계산에 적용되지 못하고 남은 시간 (ms)
        const int fps = 60;
            
        Actor model;
        ViewerCamera camera = new ViewerCamera();
        float cameraSpeed = 200f;
        float cameraAngularSpeed = (float)Math.PI * 0.5f;

        Matrix projectionMatrix;

        CubePrimitive cube;

        // TODO test
        PlanePrimitive plane;
        bool senseState = false;


        public Game1(IntPtr handle, double screenWidth, double screenHeight)
        {
            graphics = new GraphicsDeviceManager(this);
            Content.RootDirectory = "Content";
            m_WindowHandle = handle;
            this.screenWidth = screenWidth;
            this.screenHeight = screenHeight;

        }

        /// <summary>
        /// Allows the game to perform any initialization it needs to before starting to run.
        /// This is where it can query for any required services and load any non-graphic
        /// related content.  Calling base.Initialize will enumerate through any components
        /// and initialize them as well.
        /// </summary>
        protected override void Initialize()
        {
            // TODO: Add your initialization logic here
            projectionMatrix = Matrix.CreatePerspectiveFieldOfView(
                MathHelper.ToRadians(45.0f),
                //graphics.GraphicsDevice.Viewport.AspectRatio,
                (float)(screenWidth / screenHeight),
                1.0f,
                10000.0f);

            base.Initialize();

            Form gameWindowForm = (Form)Form.FromHandle(this.Window.Handle);
            gameWindowForm.Shown += new EventHandler(gameWindowForm_Shown);

            // 키넥트 초기화
            initializeKinects();
        }

        private void initializeKinects()
        {
            // 초기화
            kinects = new ArrayList();

            // 연결된 키넥트를 추가한다.
            foreach (var potentialSensor in KinectSensor.KinectSensors)
            {
                if (potentialSensor.Status == KinectStatus.Connected)
                {
                    kinects.Add(potentialSensor);
                    break;
                }
            }

            // copyOfDepthPixels = new short[1070];
            // TODO 일단 하나의 키넥트만. 추후 여러개 사용하게 함.
            if (kinects.Count > 0)
            {
                sensor = (KinectSensor)kinects[kinects.Count - 1];
                if (sensor != null)
                {
                    // depth frame을 받을 수 있게 함
                    sensor.DepthStream.Enable(DepthImageFormat.Resolution640x480Fps30);

                    // depth pixel 정보를 넣을 공간을 할당
                    depthPixels = new short[sensor.DepthStream.FramePixelDataLength];
                    copyOfDepthPixels = new short[depthPixels.Length];

                    // 리스너 등록
                    sensor.DepthFrameReady += onDepthFrameReady;

                    // 키넥트 시작
                    try
                    {
                        sensor.Start();
                    }
                    catch (IOException e)
                    {
                        sensor = null;
                    }
                }
            }

        }

        private void onDepthFrameReady(object sender, DepthImageFrameReadyEventArgs e)
        {
            senseState = true;

            using (DepthImageFrame depthFrame = e.OpenDepthImageFrame())
            {
                if (depthFrame != null)
                {
                    // 버퍼에 복사
                    depthFrame.CopyPixelDataTo(depthPixels);
                }
            }

            senseState = false;
        }

        void gameWindowForm_Shown(object sender, EventArgs e)
        {
            ((Form)sender).Hide();
        }

        /// <summary>
        /// LoadContent will be called once per game and is the place to load
        /// all of your content.
        /// </summary>
        protected override void LoadContent()
        {
            // Create a new SpriteBatch, which can be used to draw textures.
            spriteBatch = new SpriteBatch(GraphicsDevice);

            // TODO: use this.Content to load your game content here
            // TODO test
            Animation3D animation = XmlImporter.Instance.import("../../test.xml");
            XmlExporter.Instance.export("../../output.xml", animation);

            model = new Actor(GraphicsDevice);
            model.setCurAnimation(animation);

            cube = new CubePrimitive(GraphicsDevice, new Vector3(1, 1, 1), new Vector3(0.0f, 0.0f, 0.0f));
            plane = new PlanePrimitive(GraphicsDevice);

            camera.Center = new Vector3(0, 0, 0);
            camera.Distance = 500;
        }

        /// <summary>
        /// UnloadContent will be called once per game and is the place to unload
        /// all content.
        /// </summary>
        protected override void UnloadContent()
        {
            // TODO: Unload any non ContentManager content 
        }

        /// <summary>
        /// Allows the game to run logic such as updating the world,
        /// checking for collisions, gathering input, and playing audio.
        /// </summary>
        /// <param name="gameTime">Provides a snapshot of timing values.</param>
        protected override void Update(GameTime gameTime)
        {
            // Allows the game to exit
            if (GamePad.GetState(PlayerIndex.One).Buttons.Back == Microsoft.Xna.Framework.Input.ButtonState.Pressed)
                this.Exit();

            // TODO: Add your update logic here
            // 동기화
            int deltaTime = gameTime.ElapsedGameTime.Milliseconds;
            int deltaFrame = (remainderTime + deltaTime) * fps / 1000;
            remainderTime = (remainderTime + deltaTime) - deltaFrame * 1000 / fps;

            inputProcess((float)deltaTime / 1000f);

            if (deltaFrame > 0)
            {
                //model.update(1);

                base.Update(gameTime);
            }
        }

        float angle = 0.0f;
        public float deltaAngle = 0.0f;

        /// <summary>
        /// This is called when the game should draw itself.
        /// </summary>
        /// <param name="gameTime">Provides a snapshot of timing values.</param>
        protected override void Draw(GameTime gameTime)
        {
            GraphicsDevice.Clear(Color.CornflowerBlue);

            // TODO: Add your drawing code here
            angle += deltaAngle;

            Matrix world = Matrix.CreateRotationY(angle) * Matrix.CreateTranslation(Vector3.Zero);
            Matrix projection = projectionMatrix;
            Matrix view = camera.ViewMatrix;//cameraViewMatrix;
            //cube.Draw(world, view, projection, new Color(255, 0, 0, 255));
            model.Rotation = new Vector3(0, angle, 0);


            //model.draw(view, projection, new Color(255, 0, 0, 255));

            // depth 정보 그리기
            if (senseState == false)
            {
                for (int i = 0; i < copyOfDepthPixels.Length; ++i)
                {
                    copyOfDepthPixels[i] = depthPixels[i];
                }
            }
            //for (int i = 0; i < copyOfDepthPixels.Length; ++i)
            for (int i = 0; i < copyOfDepthPixels.Length; ++i)
            {
                //*
                plane.Draw(
                        Matrix.CreateTranslation(
                            i % sensor.DepthStream.FrameWidth - sensor.DepthStream.FrameWidth/2,
                            i / sensor.DepthStream.FrameWidth - sensor.DepthStream.FrameHeight/2,
                            (copyOfDepthPixels[i] >> DepthImageFrame.PlayerIndexBitmaskWidth + 1) & 255
                        ),
                        view,
                        projection,
                        Color.Green
                    );
                //*/
                /*
                plane.Draw(
                        Matrix.CreateTranslation(
                            i % 50 - 25,
                            i / 50 - 25,
                            (copyOfDepthPixels[i] >> DepthImageFrame.PlayerIndexBitmaskWidth + 1) & 255
                        ),
                        view,
                        projection,
                        Color.Green
                    );
                //*/
            }


            GraphicsDevice.Present(null, null, m_WindowHandle);

            base.Draw(gameTime);
        }

        private void inputProcess(float deltaTime)
        {
            keyboardProcess(deltaTime);
            mouseProcess(deltaTime);
        }

        private void keyboardProcess(float deltaTime)
        {
            // 카메라 각도
            if (Keyboard.GetState().IsKeyDown(Microsoft.Xna.Framework.Input.Keys.W))
            {
                camera.ViewerAngle = new Vector3(camera.ViewerAngle.X + cameraAngularSpeed*deltaTime, camera.ViewerAngle.Y, camera.ViewerAngle.Z);
            }
            if (Keyboard.GetState().IsKeyDown(Microsoft.Xna.Framework.Input.Keys.S))
            {
                camera.ViewerAngle = new Vector3(camera.ViewerAngle.X - cameraAngularSpeed * deltaTime, camera.ViewerAngle.Y, camera.ViewerAngle.Z);
            }
            if (Keyboard.GetState().IsKeyDown(Microsoft.Xna.Framework.Input.Keys.A))
            {
                camera.ViewerAngle = new Vector3(camera.ViewerAngle.X, camera.ViewerAngle.Y - cameraAngularSpeed * deltaTime, camera.ViewerAngle.Z);
            }
            if (Keyboard.GetState().IsKeyDown(Microsoft.Xna.Framework.Input.Keys.D))
            {
                camera.ViewerAngle = new Vector3(camera.ViewerAngle.X, camera.ViewerAngle.Y + cameraAngularSpeed * deltaTime, camera.ViewerAngle.Z);
            }

            // 카메라 거리
            if (Keyboard.GetState().IsKeyDown(Microsoft.Xna.Framework.Input.Keys.R))
            {
                camera.Distance -= cameraSpeed * deltaTime;
            }
            if (Keyboard.GetState().IsKeyDown(Microsoft.Xna.Framework.Input.Keys.F))
            {
                camera.Distance += cameraSpeed * deltaTime;
            }
        }

        private void mouseProcess(float deltaTime)
        {
        }


    }
}
