#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include <chrono>
#include <map>
#include <Windows.h>
#include "Entity.h"
#include "OS-ImGui/imgui/imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "OS-ImGui/imgui/imgui_internal.h"


class GUI {
public:
	GUI() noexcept;
	[[nodiscard]] bool isOpen() const noexcept { return open; }

private:
	bool open = true;
};
inline std::optional<GUI> gui;


namespace Render
{
	void DrawFovCircle(const CEntity& LocalEntity)
	{
		if (!MenuConfig::DrawFov)
			return;

		Vec2 CenterPoint = Gui.Window.Size / 2;
		float Radius = tan(AimControl::AimFov / 180.f * M_PI / 2.f) / tan(LocalEntity.Pawn.Fov / 180.f * M_PI / 2.f) * Gui.Window.Size.x;
		Gui.Circle(CenterPoint, Radius, MenuConfig::FovCircleColor, 1);
	}

	void DrawCrossHair(ImDrawList* drawList, const ImVec2& pos, ImU32 color) noexcept
	{
		int BorderWidth = 2;
		int DotSize = 1;
		int gap = CrosshairConfig::Gap / 2;

		int outlineGap = gap - 1;

		ImVec2 offset1{ CrosshairConfig::DotSize,CrosshairConfig::DotSize };
		ImVec2 offset2{ CrosshairConfig::DotSize + 1,CrosshairConfig::DotSize + 1 };

		/*
		===== Outline =====
		*/
		if (CrosshairConfig::drawOutLine)
		{
			//dot
			if (CrosshairConfig::drawDot)
				drawList->AddRectFilled(ImVec2(pos.x - offset1.x, pos.y - offset1.y), ImVec2(pos.x + offset2.x, pos.y + offset2.y), color & IM_COL32_A_MASK);

			if (CrosshairConfig::drawCrossline)
			{
				//left
				drawList->AddRectFilled(ImVec2(pos.x - (outlineGap + BorderWidth + CrosshairConfig::HorizontalLength), pos.y - 1), ImVec2(pos.x - outlineGap, pos.y + 2), color & IM_COL32_A_MASK);
				//right
				drawList->AddRectFilled(ImVec2(pos.x + (outlineGap + DotSize), pos.y - 1), ImVec2(pos.x + (outlineGap + DotSize + BorderWidth + CrosshairConfig::HorizontalLength), pos.y + 2), color & IM_COL32_A_MASK);
				//top
				if (!CrosshairConfig::tStyle)
					drawList->AddRectFilled(ImVec2(pos.x - 1, pos.y - (outlineGap + BorderWidth + CrosshairConfig::VerticalLength)), ImVec2(pos.x + 2, pos.y - outlineGap), color & IM_COL32_A_MASK);
				//bottom
				drawList->AddRectFilled(ImVec2(pos.x - 1, pos.y + outlineGap + DotSize), ImVec2(pos.x + 2, pos.y + (outlineGap + DotSize + BorderWidth + CrosshairConfig::VerticalLength)), color & IM_COL32_A_MASK);
			}
						
			//circle
			if (CrosshairConfig::drawCircle)
				drawList->AddCircle(ImVec2(pos.x, pos.y), CrosshairConfig::CircleRadius, color & IM_COL32_A_MASK, 0, 3.0f);
		}

		/*
		===== Crosshair =====
		*/
		// dot
		if (CrosshairConfig::drawDot)
			drawList->AddRectFilled(ImVec2(pos.x - offset1.x + DotSize, pos.y - offset1.y + DotSize), ImVec2(pos.x + offset1.x, pos.y + offset1.y), color);

		if (CrosshairConfig::drawCrossline)
		{
			// left
			drawList->AddRectFilled(ImVec2(pos.x - (gap + CrosshairConfig::HorizontalLength), pos.y), ImVec2(pos.x - gap, pos.y + 1), color);
			// right
			drawList->AddRectFilled(ImVec2(pos.x + gap + DotSize, pos.y), ImVec2(pos.x + (gap + DotSize + CrosshairConfig::HorizontalLength), pos.y + 1), color);
			// top
			if (!CrosshairConfig::tStyle)
				drawList->AddRectFilled(ImVec2(pos.x, pos.y - (gap + CrosshairConfig::VerticalLength)), ImVec2(pos.x + 1, pos.y - gap), color);
			// bottom
			drawList->AddRectFilled(ImVec2(pos.x, pos.y + gap + DotSize), ImVec2(pos.x + 1, pos.y + (gap + DotSize + CrosshairConfig::VerticalLength)), color);
		}
		
		// circle
		if (CrosshairConfig::drawCircle)
			drawList->AddCircle(ImVec2(pos.x, pos.y), CrosshairConfig::CircleRadius, color, 0, 1.0f);
	}

	void DrawCrossHair2D()
	{
		Vec2 SightPos = Gui.Window.Size / 2;
		Gui.Line({ SightPos.x - CrosshairConfig::CrossHairSize,SightPos.y }, { SightPos.x + CrosshairConfig::CrossHairSize,SightPos.y }, CrosshairConfig::CrossHairColor, 1);
		Gui.Line({ SightPos.x,SightPos.y - CrosshairConfig::CrossHairSize }, { SightPos.x ,SightPos.y + CrosshairConfig::CrossHairSize }, CrosshairConfig::CrossHairColor, 1);
	}

	void LineToEnemy(ImVec4 Rect, ImColor Color, float Thickness)
	{
		switch (MenuConfig::LinePos)
		{
		case 0:
			Gui.Line({ Rect.x + Rect.z / 2,Rect.y }, { Gui.Window.Size.x / 2,0 }, Color, Thickness);
			break;
		case 1:
			Gui.Line({ Rect.x + Rect.z / 2,Rect.y }, { Gui.Window.Size.x / 2, Gui.Window.Size.y/2 }, Color, Thickness);
			break;
		case 2:
			Gui.Line({ Rect.x + Rect.z / 2,Rect.y }, { Gui.Window.Size.x / 2, Gui.Window.Size.y }, Color, Thickness);
			break;
		default:
			break;
		}
		
	}

	void DrawFov(const CEntity& LocalEntity, float Size, ImColor Color, float Thickness)
	{
		if (!MenuConfig::ShowFovLine)
			return;

		float Length;
		float radian;
		Vec2 LineEndPoint[2];
		Vec2 Pos = Gui.Window.Size / 2;

		radian = (LocalEntity.Pawn.Fov / 2) * M_PI / 180;

		LineEndPoint[0].y = Pos.y - Size;
		LineEndPoint[1].y = LineEndPoint[0].y;

		Length = Size * tan(radian);

		LineEndPoint[0].x = Pos.x - Length;
		LineEndPoint[1].x = Pos.x + Length;

		Gui.Line(Pos, LineEndPoint[0], Color, 1.5);
		Gui.Line(Pos, LineEndPoint[1], Color, 1.5);
	}

	void HeadShootLine(const CEntity& LocalEntity, ImColor Color)
	{
		if (!MenuConfig::ShowHeadShootLine)
			return;

		Vec2 Pos;
		Pos.x = Gui.Window.Size.x / 2;
		Pos.y = Gui.Window.Size.y / 2.0f - Gui.Window.Size.y / (2.0f * std::sin(LocalEntity.Pawn.Fov * M_PI / 180.0f) / std::sin(90.0f * M_PI / 180.0f)) * std::sin(LocalEntity.Pawn.ViewAngle.x * M_PI / 180.0f) / std::sin(90.0f * M_PI / 180.0f);

		// left
		Gui.RectangleFilled(Vec2{ Pos.x - 21, Pos.y - 1 }, Vec2{ 17, 3}, Color & IM_COL32_A_MASK);
		Gui.RectangleFilled(Vec2{ Pos.x - 20, Pos.y }, Vec2{ 17, 3 }, Color);

		// right
		Gui.RectangleFilled(Vec2{ Pos.x + 5, Pos.y - 1 }, Vec2{ 17, 3 }, Color & IM_COL32_A_MASK);
		Gui.RectangleFilled(Vec2{ Pos.x + 6, Pos.y }, Vec2{ 17, 3 }, Color);
	}

	// 方框绘制
	ImVec4 Get2DBox(const CEntity& Entity)
	{
		BoneJointPos Head = Entity.GetBone().BonePosList[BONEINDEX::head];

		Vec2 Size, Pos;
		Size.y = (Entity.Pawn.ScreenPos.y - Head.ScreenPos.y) * 1.09;
		Size.x = Size.y * 0.6;

		Pos = ImVec2(Entity.Pawn.ScreenPos.x - Size.x / 2, Head.ScreenPos.y- Size.y*0.08);

		return ImVec4{ Pos.x,Pos.y,Size.x,Size.y };
	}

	// 骨骼绘制
	void DrawBone(const CEntity& Entity, ImColor Color, float Thickness)
	{
		if (!MenuConfig::ShowBoneESP)
			return;

		BoneJointPos Previous, Current;
		for (auto i : BoneJointList::List)
		{
			Previous.Pos = Vec3(0, 0, 0);
			for (auto Index : i)
			{
				Current = Entity.GetBone().BonePosList[Index];
				if (Previous.Pos == Vec3(0, 0, 0))
				{
					Previous = Current;
					//pGame->View->Gui->Text(Current.Name.c_str(), Current.ScreenPos, ImColor(255, 255, 0, 255));
					continue;
				}
				if (Previous.IsVisible && Current.IsVisible)
				{
					Gui.Line(Previous.ScreenPos, Current.ScreenPos, Color, Thickness);
					//pGame->View->Gui->Text(Current.Name.c_str(), Current.ScreenPos, ImColor(255, 255, 0, 255));
				}
				Previous = Current;
			}
		}
	}

	// 朝向绘制
	void ShowLosLine(const CEntity& Entity, const float Length, ImColor Color, float Thickness)
	{
		if (!MenuConfig::ShowEyeRay)
			return;

		Vec2 StartPoint, EndPoint;
		Vec3 Temp;
		BoneJointPos Head = Entity.GetBone().BonePosList[BONEINDEX::head];

		StartPoint = Head.ScreenPos;

		float LineLength = cos(Entity.Pawn.ViewAngle.x * M_PI / 180) * Length;

		Temp.x = Head.Pos.x + cos(Entity.Pawn.ViewAngle.y * M_PI / 180) * LineLength;
		Temp.y = Head.Pos.y + sin(Entity.Pawn.ViewAngle.y * M_PI / 180) * LineLength;
		Temp.z = Head.Pos.z - sin(Entity.Pawn.ViewAngle.x * M_PI / 180) * Length;

		if (!gGame.View.WorldToScreen(Temp, EndPoint))
			return;

		Gui.Line(StartPoint, EndPoint, Color, Thickness);
	}

	//牛牛绘制
	void ShowPenis(const CEntity& Entity, const float Length, ImColor Color, float Thickness)
	{
		if (!MenuConfig::ShowPenis)
			return;

		Vec2 StartPoint, EndPoint;
		Vec3 Temp;
		BoneJointPos Head = Entity.GetBone().BonePosList[BONEINDEX::pelvis];

		StartPoint = Head.ScreenPos;

		float LineLength = cos(Entity.Pawn.ViewAngle.x * M_PI / 180) * Length;

		Temp.x = Head.Pos.x + cos(Entity.Pawn.ViewAngle.y * M_PI / 180) * LineLength;
		Temp.y = Head.Pos.y + sin(Entity.Pawn.ViewAngle.y * M_PI / 180) * LineLength;
		Temp.z = Head.Pos.z - sin(Entity.Pawn.ViewAngle.x * M_PI / 180) * Length;

		if (!gGame.View.WorldToScreen(Temp, EndPoint))
			return;

		Gui.Line(StartPoint, EndPoint, Color, Thickness);
	}

	// 2D骨骼框绘制
	ImVec4 Get2DBoneRect(const CEntity& Entity)
	{
		Vec2 Min, Max, Size;
		Min = Max = Entity.GetBone().BonePosList[0].ScreenPos;

		for (auto &BoneJoint : Entity.GetBone().BonePosList)
		{
			if (!BoneJoint.IsVisible)
				continue;
			Min.x = min(BoneJoint.ScreenPos.x, Min.x);
			Min.y = min(BoneJoint.ScreenPos.y, Min.y);
			Max.x = max(BoneJoint.ScreenPos.x, Max.x);
			Max.y = max(BoneJoint.ScreenPos.y, Max.y);
		}
		Size.x = Max.x - Min.x;
		Size.y = Max.y - Min.y;

		return ImVec4(Min.x, Min.y, Size.x, Size.y);
	}

	class HealthBar
	{
	private:
		using TimePoint_ = std::chrono::steady_clock::time_point;
	private:
		// 显示备份血条时间(ms)
		const int ShowBackUpHealthDuration = 500;
		// 最大血量
		float MaxHealth = 0.f;
		// 当前血量
		float CurrentHealth = 0.f;
		// 最近备份血量大小
		float LastestBackupHealth = 0.f;
		// 血条坐标
		ImVec2 RectPos{};
		// 血条大小
		ImVec2 RectSize{};
		// 正在显示备份血量
		bool InShowBackupHealth = false;
		// 显示备份血量起始时间戳
		TimePoint_ BackupHealthTimePoint{};
	public:
		HealthBar() {}
		// 横向
		void DrawHealthBar_Horizontal(float MaxHealth, float CurrentHealth, ImVec2 Pos, ImVec2 Size);
		// 纵向
		void DrawHealthBar_Vertical(float MaxHealth, float CurrentHealth, ImVec2 Pos, ImVec2 Size);
	private:
		// 颜色缓动
		ImColor Mix(ImColor Col_1, ImColor Col_2, float t);
		// 第一阶段血条颜色 0.5-1
		ImColor FirstStageColor = ImColor(96, 246, 113, 220);
		// 第二阶段血条颜色 0.5-0.2
		ImColor SecondStageColor = ImColor(247, 214, 103, 220);
		// 第三阶段血条颜色 0.2-0.0
		ImColor ThirdStageColor = ImColor(255, 95, 95, 220);
		// 备份血条颜色
		ImColor BackupHealthColor = ImColor(255, 255, 255, 220);
		// 边框颜色
		ImColor FrameColor = ImColor(45, 45, 45, 220);
		// 背景颜色
		ImColor BackGroundColor = ImColor(90, 90, 90, 220);
	};

	void HealthBar::DrawHealthBar_Horizontal(float MaxHealth, float CurrentHealth, ImVec2 Pos, ImVec2 Size)
	{
		auto InRange = [&](float value, float min, float max) -> bool
		{
			return value > min && value <= max;
		};

		ImDrawList* DrawList = ImGui::GetBackgroundDrawList();

		this->MaxHealth = MaxHealth;
		this->CurrentHealth = CurrentHealth;
		this->RectPos = Pos;
		this->RectSize = Size;

		// 占比
		float Proportion = CurrentHealth / MaxHealth;
		// 血量条宽度
		float Width = RectSize.x * Proportion;
		// 血量条颜色
		ImColor Color;

		// 背景
		DrawList->AddRectFilled(RectPos,
			{ RectPos.x + RectSize.x,RectPos.y + RectSize.y },
			BackGroundColor, 5, 15);

		// 颜色切换
		float Color_Lerp_t = pow(Proportion, 2.5);
		if (InRange(Proportion, 0.5, 1))
			Color = Mix(FirstStageColor, SecondStageColor, Color_Lerp_t * 3 - 1);
		else
			Color = Mix(SecondStageColor, ThirdStageColor, Color_Lerp_t * 4);

		// 更新最近备份血量
		if (LastestBackupHealth == 0
			|| LastestBackupHealth < CurrentHealth)
			LastestBackupHealth = CurrentHealth;

		if (LastestBackupHealth != CurrentHealth)
		{
			if (!InShowBackupHealth)
			{
				BackupHealthTimePoint = std::chrono::steady_clock::now();
				InShowBackupHealth = true;
			}

			std::chrono::steady_clock::time_point CurrentTime = std::chrono::steady_clock::now();
			if (CurrentTime - BackupHealthTimePoint > std::chrono::milliseconds(ShowBackUpHealthDuration))
			{
				// 超时就停止显示备份血量，并且更新最近备份血量
				LastestBackupHealth = CurrentHealth;
				InShowBackupHealth = false;
			}

			if (InShowBackupHealth)
			{
				// 备份血量绘制宽度
				float BackupHealthWidth = LastestBackupHealth / MaxHealth * RectSize.x;
				// 备份血量alpha渐变
				float BackupHealthColorAlpha = 1 - 0.95 * (std::chrono::duration_cast<std::chrono::milliseconds>(CurrentTime - BackupHealthTimePoint).count() / (float)ShowBackUpHealthDuration);
				ImColor BackupHealthColorTemp = BackupHealthColor;
				BackupHealthColorTemp.Value.w = BackupHealthColorAlpha;
				// 备份血量宽度缓动
				float BackupHealthWidth_Lerp = 1 * (std::chrono::duration_cast<std::chrono::milliseconds>(CurrentTime - BackupHealthTimePoint).count() / (float)ShowBackUpHealthDuration);
				BackupHealthWidth_Lerp *= (BackupHealthWidth - Width);
				BackupHealthWidth -= BackupHealthWidth_Lerp;
				// 备份血条
				DrawList->AddRectFilled(RectPos,
					{ RectPos.x + BackupHealthWidth,RectPos.y + RectSize.y },
					BackupHealthColorTemp, 5);
			}
		}

		// 血条
		DrawList->AddRectFilled(RectPos,
			{ RectPos.x + Width,RectPos.y + RectSize.y },
			Color, 5);

		// 边框
		DrawList->AddRect(RectPos,
			{ RectPos.x + RectSize.x,RectPos.y + RectSize.y },
			FrameColor, 5, 15, 1);
	}

	void HealthBar::DrawHealthBar_Vertical(float MaxHealth, float CurrentHealth, ImVec2 Pos, ImVec2 Size)
	{
		auto InRange = [&](float value, float min, float max) -> bool
		{
			return value > min && value <= max;
		};

		ImDrawList* DrawList = ImGui::GetBackgroundDrawList();

		this->MaxHealth = MaxHealth;
		this->CurrentHealth = CurrentHealth;
		this->RectPos = Pos;
		this->RectSize = Size;

		// 占比
		float Proportion = CurrentHealth / MaxHealth;
		// 血量条高度
		float Height = RectSize.y * Proportion;
		// 血量条颜色
		ImColor Color;

		// 背景
		DrawList->AddRectFilled(RectPos,
			{ RectPos.x + RectSize.x,RectPos.y + RectSize.y },
			BackGroundColor, 5, 15);

		// 颜色切换
		float Color_Lerp_t = pow(Proportion, 2.5);
		if (InRange(Proportion, 0.5, 1))
			Color = Mix(FirstStageColor, SecondStageColor, Color_Lerp_t * 3 - 1);
		else
			Color = Mix(SecondStageColor, ThirdStageColor, Color_Lerp_t * 4);

		// 更新最近备份血量
		if (LastestBackupHealth == 0
			|| LastestBackupHealth < CurrentHealth)
			LastestBackupHealth = CurrentHealth;

		if (LastestBackupHealth != CurrentHealth)
		{
			if (!InShowBackupHealth)
			{
				BackupHealthTimePoint = std::chrono::steady_clock::now();
				InShowBackupHealth = true;
			}

			std::chrono::steady_clock::time_point CurrentTime = std::chrono::steady_clock::now();
			if (CurrentTime - BackupHealthTimePoint > std::chrono::milliseconds(ShowBackUpHealthDuration))
			{
				// 超时就停止显示备份血量，并且更新最近备份血量
				LastestBackupHealth = CurrentHealth;
				InShowBackupHealth = false;
			}

			if (InShowBackupHealth)
			{
				// 备份血量绘制高度
				float BackupHealthHeight = LastestBackupHealth / MaxHealth * RectSize.y;
				// 备份血量alpha渐变
				float BackupHealthColorAlpha = 1 - 0.95 * (std::chrono::duration_cast<std::chrono::milliseconds>(CurrentTime - BackupHealthTimePoint).count() / (float)ShowBackUpHealthDuration);
				ImColor BackupHealthColorTemp = BackupHealthColor;
				BackupHealthColorTemp.Value.w = BackupHealthColorAlpha;
				// 备份血量高度缓动
				float BackupHealthHeight_Lerp = 1 * (std::chrono::duration_cast<std::chrono::milliseconds>(CurrentTime - BackupHealthTimePoint).count() / (float)ShowBackUpHealthDuration);
				BackupHealthHeight_Lerp *= (BackupHealthHeight - Height);
				BackupHealthHeight -= BackupHealthHeight_Lerp;
				// 备份血条
				DrawList->AddRectFilled({ RectPos.x,RectPos.y + RectSize.y - BackupHealthHeight },
					{ RectPos.x + RectSize.x,RectPos.y + RectSize.y },
					BackupHealthColorTemp, 5);
			}
		}

		// 血条
		DrawList->AddRectFilled({ RectPos.x,RectPos.y + RectSize.y - Height },
			{ RectPos.x + RectSize.x,RectPos.y + RectSize.y },
			Color, 5);

		// 边框
		DrawList->AddRect(RectPos,
			{ RectPos.x + RectSize.x,RectPos.y + RectSize.y },
			FrameColor, 5, 15, 1);
	}

	ImColor HealthBar::Mix(ImColor Col_1, ImColor Col_2, float t)
	{
		ImColor Col;
		Col.Value.x = t * Col_1.Value.x + (1 - t) * Col_2.Value.x;
		Col.Value.y = t * Col_1.Value.y + (1 - t) * Col_2.Value.y;
		Col.Value.z = t * Col_1.Value.z + (1 - t) * Col_2.Value.z;
		Col.Value.w = Col_1.Value.w;
		return Col;
	}

	// Sign可用任何类型敌人标识，默认可传敌人地址
	void DrawHealthBar(DWORD Sign, float MaxHealth, float CurrentHealth, ImVec2 Pos, ImVec2 Size, bool Horizontal)
	{
		static std::map<DWORD, HealthBar> HealthBarMap;
		if (!HealthBarMap.count(Sign))
		{
			HealthBarMap.insert({ Sign,HealthBar() });
		}
		if (HealthBarMap.count(Sign))
		{
			if (Horizontal)
				HealthBarMap[Sign].DrawHealthBar_Horizontal(MaxHealth, CurrentHealth, Pos, Size);
			else
				HealthBarMap[Sign].DrawHealthBar_Vertical(MaxHealth, CurrentHealth, Pos, Size);
		}
	}

	// Update window style color
	void UpdateStyle(int Style)
	{
		switch (Style) {
		case 0:
			ImGui::StyleColorsDark();
			break;
		case 1:
			ImGui::StyleColorsEnemyMouse();
			break;
		case 2:
			ImGui::StyleColorsClassic();
			break;
		}
	}

	// Update crosshair preset
	void UpdateCrosshairPreset(int style)
	{
		switch (style) {
		case 1:
			CrosshairConfig::drawDot = true;
			CrosshairConfig::DotSize = 2;
			CrosshairConfig::drawOutLine = true;
			CrosshairConfig::drawCrossline = false;
			CrosshairConfig::drawCircle = false;
			CrosshairConfig::showTargeting = true;
			break;
		case 2:
			CrosshairConfig::drawDot = true;
			CrosshairConfig::DotSize = 2;
			CrosshairConfig::drawOutLine = true;
			CrosshairConfig::drawCrossline = false;
			CrosshairConfig::drawCircle = true;
			CrosshairConfig::CircleRadius = 10.f;
			CrosshairConfig::showTargeting = true;
			break;
		case 3:
			CrosshairConfig::drawDot = true;
			CrosshairConfig::DotSize = 1;
			CrosshairConfig::drawOutLine = true;
			CrosshairConfig::drawCrossline = true;
			CrosshairConfig::HorizontalLength = 5;
			CrosshairConfig::VerticalLength = 5;
			CrosshairConfig::Gap = 18;
			CrosshairConfig::tStyle = false;
			CrosshairConfig::drawCircle = true;
			CrosshairConfig::CircleRadius = 10.f;
			CrosshairConfig::showTargeting = true;
			break;
		case 4:
			CrosshairConfig::drawDot = true;
			CrosshairConfig::DotSize = 1;
			CrosshairConfig::drawOutLine = true;
			CrosshairConfig::drawCrossline = true;
			CrosshairConfig::HorizontalLength = 5;
			CrosshairConfig::VerticalLength = 5;
			CrosshairConfig::Gap = 1;
			CrosshairConfig::tStyle = false;
			CrosshairConfig::drawCircle = false;
			CrosshairConfig::showTargeting = true;
			break;
		case 5:
			CrosshairConfig::drawDot = false;
			CrosshairConfig::drawOutLine = true;
			CrosshairConfig::drawCrossline = true;
			CrosshairConfig::HorizontalLength = 7;
			CrosshairConfig::VerticalLength = 7;
			CrosshairConfig::Gap = 10;
			CrosshairConfig::tStyle = false;
			CrosshairConfig::drawCircle = false;
			CrosshairConfig::showTargeting = true;
			break;
		case 6:
			CrosshairConfig::drawDot = true;
			CrosshairConfig::DotSize = 2;
			CrosshairConfig::drawOutLine = true;
			CrosshairConfig::drawCrossline = true;
			CrosshairConfig::HorizontalLength = 8;
			CrosshairConfig::VerticalLength = 8;
			CrosshairConfig::Gap = 20;
			CrosshairConfig::tStyle = false;
			CrosshairConfig::drawCircle = false;
			CrosshairConfig::showTargeting = true;
			break;
		default:
			break;
		}
	}

	// Get the center pos of screen
	ImVec2 GetScreenCenterImVec2()
	{
		int W = GetSystemMetrics(SM_CXSCREEN);
		int H = GetSystemMetrics(SM_CYSCREEN);

		float CenterX = static_cast<float>(W) / 2;
		float CenterY = static_cast<float>(H) / 2;

		return ImVec2(CenterX, CenterY);
	}

	// Convert RGBA to ImVec4
	// Why not use "ImColor()"? Because I forgot about this.
	ImVec4 rgba2ImVec(int r, int g, int b, int a)
	{
		ImVec4 color;
		color.x = static_cast<float>(r) / 255.0f;
		color.y = static_cast<float>(g) / 255.0f;
		color.z = static_cast<float>(b) / 255.0f;
		color.w = static_cast<float>(a) / 255.0f;

		return color;
	}
}