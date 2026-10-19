// 출처: https://powerclabman.tistory.com/152  [그래픽스] 백 페이스 컬링
// 코드 스크린샷(이미지 5~7)을 보고 옮겨 적은 코드

// ----- 이미지 5 -----
//백페이스 컬링
inline bool IsBackFace(const Vector3D& first, const Vector3D& second, const Vector3D& third)
{
	Vector3D one = second - first;
	Vector3D two = third - second;

	Vector3D normalVector = one.Cross(two);

	if (m_camera3D.GetCameraLook().Dot(normalVector) >= 0.f)
	{
		return true;
	}

	return false;
}

// ----- 이미지 6: Render3D 정점 처리 루프에 컬링 추가 (나머지는 151번 글과 동일) -----
		//정점 처리
		for (const MeshIndex& idx : iBuffer)
		{
			const Vertex3D& fst = vBuffer[idx.First];
			const Vertex3D& sec = vBuffer[idx.Second];
			const Vertex3D& trd = vBuffer[idx.Third];

			//백페이스 컬링
			if (IsBackFace(fst.Position, sec.Position, trd.Position))
			{
				continue;
			}

			array<Vertex2D, 3> vertexArray = { fst.ToVertex2D(),sec.ToVertex2D(),trd.ToVertex2D() };

			if (Engine::GetInst()->GetDebugFlag().bWireFrameRenderFlag)
			{
				DrawWireTriangle(vertexArray);
				continue;
			}

			switch (mesh->GetDrawMode())
			// ... (이후 151번 글과 동일)

// ----- 이미지 7: 와인딩 순서를 맞춘 육면체 (Dummy::Init) -----
{
	Super::Init();

	Mesh3D* mesh = GetMesh();
	m_trans = GetTransformer();
	mesh->SetDrawMode(eMeshDrawMode::WIREFRAME);
	// 정점 추가
	mesh->AddVertex(Vertex3D{ Vector3D{50.f, 50.f, 50.f}, Color32::s_blue });   // 0번 정점
	mesh->AddVertex(Vertex3D{ Vector3D{-50.f, 50.f, 50.f}, Color32::s_blue });  // 1번 정점
	mesh->AddVertex(Vertex3D{ Vector3D{-50.f, -50.f, 50.f}, Color32::s_blue }); // 2번 정점
	mesh->AddVertex(Vertex3D{ Vector3D{50.f, -50.f, 50.f}, Color32::s_blue });  // 3번 정점
	mesh->AddVertex(Vertex3D{ Vector3D{50.f, 50.f, -50.f}, Color32::s_blue });  // 4번 정점
	mesh->AddVertex(Vertex3D{ Vector3D{-50.f, 50.f, -50.f}, Color32::s_blue }); // 5번 정점
	mesh->AddVertex(Vertex3D{ Vector3D{-50.f, -50.f, -50.f}, Color32::s_blue }); // 6번 정점
	mesh->AddVertex(Vertex3D{ Vector3D{50.f, -50.f, -50.f}, Color32::s_blue }); // 7번 정점

	// 앞면 - 반시계 방향
	mesh->AddIndex(MeshIndex{ 0, 1, 2 });
	mesh->AddIndex(MeshIndex{ 2, 3, 0 });

	// 뒷면 - 시계 방향
	mesh->AddIndex(MeshIndex{ 4, 7, 6 });
	mesh->AddIndex(MeshIndex{ 6, 5, 4 });

	// 왼쪽면 - 시계 방향
	mesh->AddIndex(MeshIndex{ 1, 5, 6 });
	mesh->AddIndex(MeshIndex{ 6, 2, 1 });

	// 오른쪽면 - 반시계 방향
	mesh->AddIndex(MeshIndex{ 0, 3, 7 });
	mesh->AddIndex(MeshIndex{ 7, 4, 0 });

	// 윗면 - 반시계 방향
	mesh->AddIndex(MeshIndex{ 5,1,0 });
	mesh->AddIndex(MeshIndex{ 0,4,5 });

	// 아랫면 - 시계 방향
	mesh->AddIndex(MeshIndex{ 2, 6, 7 });
	mesh->AddIndex(MeshIndex{ 7, 3, 2 });

	m_trans->SetPosition(Vector3D(0, 0, 100));
}
