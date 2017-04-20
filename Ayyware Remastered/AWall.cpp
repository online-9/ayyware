#include "Ayywall.h"

trace_t trace;
trace_t trace2;
bool bTraceExit(Vector &vStartyPosy, Vector &vDir)
{
	float multiply = 0;
	//int v10 = 0;

	while (multiply <= 90.0f)
	{
		multiply += 4.0f;

		Vector vDaStart = (vDir * multiply) + vStartyPosy;
		Vector vDaEnd = vDaStart - (vDir * 4.0f);

		int point_contents = (Interfaces::Trace->GetPointContents(vDaStart, 0x4600400B));

		Ray_t ray2;

		if ((point_contents & 0x600400B) && (!(point_contents & 0x40000000)))
		{
			continue;
		}

		ray2.Init(vDaStart, vDaEnd);
		Interfaces::Trace->TraceRay(ray2, 0x4600400B, NULL, &trace2);

		if (trace2.startsolid && (trace2.surface.flags & 0x8000))
		{
			IClientEntity *pNewTraceEnt = trace2.m_pEnt;

			CTraceFilter tracefilter_new;  tracefilter_new.pSkip = pNewTraceEnt;

			Ray_t ray3;
			ray3.Init(vDaStart, vStartyPosy);
			Interfaces::Trace->TraceRay(ray3, 0x600400B, (ITraceFilter*)&tracefilter_new, &trace2);

			if ((trace2.fraction < 1.0f || trace2.allsolid || trace2.startsolid) && !trace2.startsolid)
			{
				vDaStart = trace2.endpos;
				return true;
			}

			continue;
		}

		if (!(trace2.fraction < 1.0f || trace2.allsolid || trace2.startsolid) || trace2.startsolid)
		{
			IClientEntity *pBreakEnt = trace.m_pEnt;

			if (pBreakEnt)
			{
				IClientEntity* pWorldEnt = Interfaces::EntList->GetClientEntity(0);

				if (pBreakEnt != pWorldEnt)
				{
					static DWORD dwEntBreakable = NULL;

					if (dwEntBreakable == NULL)
					{
						dwEntBreakable = Utilities::Memory::FindPattern("client.dll", (PBYTE)"\x55\x8B\xEC\x51\x56\x8B\xF1\x85\xF6\x74\x68", "xxxxxxxxxxx");
						Utilities::Log("EntBreakable: 0x%x", dwEntBreakable);
					}
					bool bRet;

					__asm
					{
						MOV ECX, pBreakEnt
						CALL dwEntBreakable
							MOV bRet, AL
					}
					if (bRet)
						return true;
				}
			}

			continue;
		}

		if (((trace2.surface.flags >> 7) & 1) && !((trace.surface.flags >> 7) & 1))
			continue;

		float fWanker = ((trace2.plane.normal.x * vDir.x) + (trace2.plane.normal.y * vDir.y) + (trace2.plane.normal.z * vDir.z));

		if (fWanker <= 1.0f)
		{
			float fMody = trace2.fraction * 4.0f;

			Vector vGody = vDaStart - (vDir * fMody);

			vDaStart = vGody;
			return true;
		}
	}
	return false;
}


bool bIsPointPenetrable(CSWeaponInfo *wiWeaponInfo, Vector vStart, Vector vEnd)
{
	static trace_t traceEmpty;
	static CTraceFilterNoPlayer filter;

	trace = traceEmpty;

	static ITraceFilter *pTraceFilter = (ITraceFilter*)&filter;

	Vector vSource = vStart, vDir = (vEnd - vStart), vClip;
	vDir.NormalizeInPlace();

	float fTmpLength, fSumDist, fPow;

	float iCurrentDamage = wiWeaponInfo->m_iDamage;
	//char array1[10];
	//sprintf(array1, "%f", iCurrentDamage);
	//Base::Debug::LOG(array1);

	float fDamageCheck = 1.0f;

	//kolonote:
	//new penetration: this is always 4 now ehi ehi
	int iPenetration = 4;

	static DWORD dwCliptracetoplayers = NULL;

	if (dwCliptracetoplayers == NULL)
	{
		dwCliptracetoplayers = Utilities::Memory::FindPattern("client.dll", (PBYTE)"\x53\x8B\xDC\x83\xEC\x08\x83\xE4\xF0\x83\xC4\x04\x55\x8B\x6B\x04\x89\x6C\x24\x04\x8B\xEC\x81\xEC\x00\x00\x00\x00\x8B\x43\x10", "xxxxxxxxxxxxxxxxxxxxxxxx????xxx");
		Utilities::Log("ClipTraceToPlayers: 0x%x",dwCliptracetoplayers);
	}


	float fRange = Vector(vEnd - vStart).Length();

	//ValveSDK::CTrace::CSimpleTraceFilter pLocalFilter(pLocal);

	IClientEntity *pLastHit = NULL;

	while (iCurrentDamage > 0.0f)
	{
		//Base::Debug::LOG("dmg dealed");
		Ray_t ray;
		ray.Init(vSource, vEnd);



		Interfaces::Trace->TraceRay(ray, 0x4600400B, pTraceFilter, &trace);

		vClip = (vDir * 40.0f) + vEnd;


		_asm
		{
			MOV EAX, pTraceFilter
			LEA ECX, trace
			PUSH ECX
			PUSH EAX
			PUSH 0x4600400B
			LEA EDX, vClip
			LEA ECX, vSource
			CALL dwCliptracetoplayers
			ADD ESP, 0x0C
		}

		//if (trace.m_pEnt)
		//{
		//	pLastHit = trace.m_pEnt;
		//}

		if (trace.fraction != 1.0f)
		{
			surfacedata_t *pSurfaceData = Interfaces::PhysProps->GetSurfaceData(trace.surface.surfaceProps);

			USHORT material = *(USHORT*)((DWORD)pSurfaceData + 84);
			float fPenetrationPowerModifier = *(PFLOAT)((DWORD)pSurfaceData + 76);

			//fTmpLength += trace.fraction * 8192.0f;
			if (iPenetration == 4)
				fTmpLength = fRange * trace.fraction;
			else
				fTmpLength = (fSumDist + ((fRange - fSumDist) * trace.fraction));

			fPow = (pow(wiWeaponInfo->m_flRangeModifier, (fTmpLength * 0.002f)));

			iCurrentDamage = iCurrentDamage * fPow;

			//kolonote:
			//i just called this the way IDA did idgaf ehi ehi ehi
			bool a5 = (trace.contents >> 3) & 1;

			bool v104 = (trace.surface.flags >> 7) & 1;

			bool v17 = false;

			if (iPenetration == 0 && !a5 && !v104 && material != 71 && material != 89)
				v17 = true;

			if (wiWeaponInfo->m_flPenetration <= 0.0f || iPenetration <= 0)
				v17 = true;

			trace2 = traceEmpty;

			//kolonote:
			//i know i suck at naming variables -.-''
			Vector vStartyPosy = trace.endpos;

			if (!bTraceExit(vStartyPosy, vDir)
				&& !(Interfaces::Trace->GetPointContents(vStartyPosy, 0x600400B) & 0x600400B))
			{
				break;
			}

			if (v17)
			{
				break;
			}

			surfacedata_t *pSurfaceData2 = Interfaces::PhysProps->GetSurfaceData(trace2.surface.surfaceProps);

			USHORT material2 = *(USHORT*)((DWORD)pSurfaceData2 + 84);
			float fPenetrationPowerModifer2 = *(PFLOAT)((DWORD)pSurfaceData2 + 76);

			float v70 = 0.16f;
			float v31;

			if (a5 || v104)
			{
				if (material != 89 && material != 71)
					v31 = 1.0f;
				else
				{
					v31 = 3.0f;
					v70 = 0.05f;
				}
			}
			else
			{
				if (material != 89)
				{
					if (material != 71)
					{
						if (material == 70)
						{
							v31 = wiWeaponInfo->m_flPenetration;
						}
						else
						{
							/*if (material == 70 && trace.m_pEnt)
							{
							typedef bool(__thiscall* FirstOne_t)(PVOID pThis);

							bool bFirstOne = ((FirstOne_t)(*(PDWORD)(*(PDWORD)(trace.m_pEnt) + 0x260)))(trace.m_pEnt);

							if (bFirstOne)
							{
							typedef int(__thiscall* SecondOne_t)(PVOID pThis);

							int iScndFirst = ((SecondOne_t)(*(PDWORD)(*(PDWORD)(trace.m_pEnt) + 0x15C)))(trace.m_pEnt);



							int iScndSecond = ((SecondOne_t)(*(PDWORD)(*(PDWORD)(g_Aimbot.pCurrLocalPlaya) + 0x15C)))(g_Aimbot.pCurrLocalPlaya);

							if (iScndFirst == iScndSecond)
							break;
							}
							}*/

							v31 = (fPenetrationPowerModifer2 + fPenetrationPowerModifier) * 0.5f;
							v70 = 0.16f;
						}
					}
					else
					{
						v31 = 3.0f;
						v70 = 0.05f;
					}
				}
				else
				{
					v31 = 3.0f;
					v70 = 0.05f;
				}
				if(material == 89 || material == 71)
				{
				v31 = 3.0f;
				v70 = 0.05f;
				}
				else
				v31 = (fPenetrationPowerModifer2 + fPenetrationPowerModifier) * 0.5f;
			}

			if (material == material2)
			{
				//if((material2 | 2) == 87)
				if (material2 == 87 || material2 == 85)
					v31 = 3.0f;
				else
				{
					if (material2 == 76)
						v31 = 2.0f;
				}
			}

			float v34 = 1.0f;
			v34 /= v31;

			if (v34 < 0.0f)
				v34 = 0.0f;

			float fNewTmpLength = Vector(trace2.endpos - trace.endpos).Length();

			float fMine = 3.0f;
			fMine /= wiWeaponInfo->m_flPenetration;
			fMine *= 1.25f;

			float v55 = 0.0f;

			if (fMine >= 0.0f)
				v55 = fMine;

			float v35 = v34;
			v35 *= 3.0f;
			v35 *= v55;

			v35 += (iCurrentDamage * v70);

			float fShiz = fNewTmpLength;
			fShiz *= fNewTmpLength;
			fShiz *= v34;
			fShiz /= 24.0f;

			float fBisc = (v35 + fShiz);

			float v69 = 0.0f;

			if (fBisc >= 0.0f)
				v69 = fBisc;

			float fNewy = (iCurrentDamage - v69);

			iCurrentDamage = fNewy;

			if (fNewy < fDamageCheck)
				break;

			vSource = trace2.endpos;

			fSumDist = fTmpLength + fNewTmpLength;

			--iPenetration;
		}
		else
			return true;
	}
	//Base::Debug::LOG("false");
	return false;
}